#include "../config.h"
#if ASM_ENABLE
#include "passes.h"
#include "asm_ast.h"
#include "../utils/stackmap.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

// PASS 1
void replace_pseudo_stack(stackmap *sm, operand_node *pseudo_node)
{
    if (pseudo_node->type != PSEUDO)
    {
        return;
    }
    char *iden = pseudo_node->operand.pseudo->reg_name;
    if (stackmap_getoffset(sm, iden) == -1)
    {
        // we gotta add it...
        stackmap_add(sm, iden, 4); // add 4 bytes.
    }

    pseudo_node->type = STACK; // switch this to a stack operand.
    asm_stack_node *stack = MALLOC(asm_stack_node);
    stack->offset = -stackmap_getoffset(sm, iden); // - from %rbp.
    free(pseudo_node->operand.pseudo);
    pseudo_node->operand.stack = stack;
}

void replace_pseudo_function(asm_function_node *function)
{
    if (function->sm == NULL)
    {
        function->sm = stackmap_init();
    }

    list(asm_i *) *instrs = function->instructions;
    asm_i *instr;

    // iterate through each instruction, replacing pseudo stores with stack stores...
    for (int i = 0; i < instrs->len; i++)
    {
        instr = (asm_i *)list_get(instrs, i);
        if (instr->type == INSTR_MOV)
        {
            replace_pseudo_stack(function->sm, instr->instruction.mov->src);
            replace_pseudo_stack(function->sm, instr->instruction.mov->dest);
        }
        else if (instr->type == INSTR_UNARY)
        {
            replace_pseudo_stack(function->sm, instr->instruction.unary->dest);
        }
        else if (instr->type == INSTR_BINARY)
        {
            replace_pseudo_stack(function->sm, instr->instruction.binary->src);
            replace_pseudo_stack(function->sm, instr->instruction.binary->dest);
        }
        else if (instr->type == INSTR_IDIV)
        {
            replace_pseudo_stack(function->sm, instr->instruction.idiv->divisor);
        }
        else if (instr->type == INSTR_CMP)
        {
            replace_pseudo_stack(function->sm, instr->instruction.cmp->src1);
            replace_pseudo_stack(function->sm, instr->instruction.cmp->src2);
        }
        else if (instr->type == INSTR_SETCC)
        {
            replace_pseudo_stack(function->sm, instr->instruction.setcc->dest);
        }
        else if (instr->type == INSTR_PUSH)
        {
            replace_pseudo_stack(function->sm, instr->instruction.push->op);
        }
    }
}

void replace_pseudo_pass(asm_program_node *program)
{
    for (int i = 0; i < program->functions->len; i++) {
        asm_function_node* func = (asm_function_node*)list_get(program->functions, i);
        replace_pseudo_function(func);
    }
}

// PASS 2

asm_i *asm_create_stackalloc_instr(int size)
{
    asm_stackalloc_node *node = MALLOC(asm_stackalloc_node);
    node->n_bytes = size;

    asm_i *i = MALLOC(asm_i);
    i->type = INSTR_STACKALLOC;
    i->instruction.stackalloc = node;

    return i;
}

void add_stackalloc_function_cleanup_operands_function(asm_function_node *function)
{
    if (function->sm == NULL)
    {
        printf("stackmap not initialized...\n");
        exit(1);
    }

    list(asm_i *) *instrs = function->instructions;

    // insert the stackalloc instruction at the beginning of the function
    int n_bytes = stackmap_size(function->sm);
    // we need to round up to the next multiple of 16 to maintain alignment
    n_bytes = (n_bytes % 16 == 0) ? n_bytes : ((n_bytes / 16) + 1) * 16;
    asm_i *stackalloc_instr = asm_create_stackalloc_instr(n_bytes);
    list_add(instrs, (void *)stackalloc_instr, 0);

    asm_i *instr;
    // iterate through each instruction, replacing invalid stores with registers...
    int i = 0;
    while (list_get(instrs, i) != NULL)
    {
        instr = (asm_i *)list_get(instrs, i);
        if (instr->type == INSTR_MOV)
        {
            if (instr->instruction.mov->src->type == STACK && instr->instruction.mov->dest->type == STACK)
            {
                // if both operands of a mov instruction are stack operands, we need to replace it with (mem->reg;reg->mem)
                asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, instr->instruction.mov->src, REGISTER(R10));
                asm_i *mov2 = asm_create_mov_instr(OP_4_BYTES, REGISTER(R10), instr->instruction.mov->dest);

                // replace the instruction with the new instructions
                list_remove(instrs, i);
                list_add(instrs, (void *)mov1, i);
                list_add(instrs, (void *)mov2, ++i);
            }
        }
        else if (instr->type == INSTR_IDIV)
        {
            // if we have idiv $x, for example...
            if (instr->instruction.idiv->divisor->type == IMM)
            {
                asm_i *mov = asm_create_mov_instr(OP_4_BYTES, instr->instruction.idiv->divisor, REGISTER(R10));
                asm_i *idiv = asm_create_idiv_instr(REGISTER(R10));

                list_remove(instrs, i);
                list_add(instrs, (void *)mov, i);
                list_add(instrs, (void *)idiv, ++i);
            }
        }
        else if (instr->type == INSTR_BINARY)
        {
            // addl, subl, and, or, xor we need to replace operands
            asm_binary_op op = instr->instruction.binary->op;
            if (op == ASM_ADD || op == ASM_SUB || op == ASM_AND || op == ASM_OR || op == ASM_XOR)
            {
                if (instr->instruction.binary->src->type == STACK && instr->instruction.binary->dest->type == STACK)
                {
                    // addl stack, stack -> movl stack, reg; addl reg, stack
                    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, instr->instruction.binary->src, REGISTER(R10));
                    asm_i *binary = asm_create_binary_instr(instr->instruction.binary->op, REGISTER(R10), instr->instruction.binary->dest);

                    list_remove(instrs, i);
                    list_add(instrs, (void *)mov, i);
                    list_add(instrs, (void *)binary, ++i);
                }
            }
            else if (op == ASM_MULT)
            {
                // imull. dest must be register.
                // imull X, stack -> movl stack, reg; imull X, reg; movl reg, stack
                asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, instr->instruction.binary->dest, REGISTER(R11));
                asm_i *imull = asm_create_binary_instr(ASM_MULT, instr->instruction.binary->src, REGISTER(R11));
                asm_i *mov2 = asm_create_mov_instr(OP_4_BYTES, REGISTER(R11), instr->instruction.binary->dest);

                list_remove(instrs, i);
                list_add(instrs, (void *)mov1, i);
                list_add(instrs, (void *)imull, ++i);
                list_add(instrs, (void *)mov2, ++i);
            }
        }
        else if (instr->type == INSTR_CMP)
        {
            // both operands can't be memory operands...
            if (instr->instruction.cmp->src1->type == STACK && instr->instruction.cmp->src2->type == STACK)
            {
                asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, instr->instruction.cmp->src1, REGISTER(R10));
                asm_i *mov2 = asm_create_cmp_instr(REGISTER(R10), instr->instruction.cmp->src2);

                // replace the instruction with the new instructions
                list_remove(instrs, i);
                list_add(instrs, (void *)mov1, i);
                list_add(instrs, (void *)mov2, ++i);
            }

            // second operand of cmp can't be an immediate...
            if (instr->instruction.cmp->src2->type == IMM)
            {
                asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, instr->instruction.cmp->src2, REGISTER(R11));
                asm_i *mov2 = asm_create_cmp_instr(instr->instruction.cmp->src1, REGISTER(R11));

                // replace the instruction with the new instructions
                list_remove(instrs, i);
                list_add(instrs, (void *)mov1, i);
                list_add(instrs, (void *)mov2, ++i);
            }
        }

        // move to next instruction from previous program before pass.
        i++;
    }
}

void add_stackalloc_function_cleanup_operands(asm_program_node *program)
{
    for (int i = 0; i < program->functions->len; i++) {
        asm_function_node* func = (asm_function_node*)list_get(program->functions, i);
        add_stackalloc_function_cleanup_operands_function(func);
    }
}
#endif