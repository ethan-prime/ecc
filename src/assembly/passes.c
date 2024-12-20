#include "passes.h"
#include "asm_ast.h"
#include "../utils/stackmap.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

// PASS 1
void replace_pseudo_stack(stackmap* sm, operand_node* pseudo_node) {
    if (pseudo_node->type != PSEUDO) {
        return;
    }
    char* iden = pseudo_node->operand.pseudo->reg_name;
    if (stackmap_getoffset(sm, iden) == -1) {
        // we gotta add it...
        stackmap_add(sm, iden, 4); // add 4 bytes.
    }

    pseudo_node->type = STACK; // switch this to a stack operand.
    asm_stack_node* stack = MALLOC(asm_stack_node);
    stack->offset = stackmap_getoffset(sm, iden);
    free(pseudo_node->operand.pseudo);
    pseudo_node->operand.stack = stack;
}

void replace_pseudo_function(asm_function_node* function) {
    if (function->sm == NULL) {
        function->sm = stackmap_init();
    }

    list(asm_instruction_node*)* instrs = function->instructions;
    asm_instruction_node* instr;

    // iterate through each instruction, replacing pseudo stores with stack stores...
    for (int i = 0; i < instrs->len; i++) {
        instr = (asm_instruction_node*)list_get(instrs, i);
        if (instr->type == INSTR_MOV) {
            if (instr->instruction.mov->src->type == PSEUDO) {
                replace_pseudo_stack(function->sm, instr->instruction.mov->src);
            }
            if (instr->instruction.mov->dest->type == PSEUDO) {
                replace_pseudo_stack(function->sm, instr->instruction.mov->dest);
            }
        } else if (instr->type == INSTR_UNARY) {
            if (instr->instruction.unary->dest->type == PSEUDO) {
                replace_pseudo_stack(function->sm, instr->instruction.unary->dest);
            }
        }
    }
}

void replace_pseudo_pass(asm_program_node* program) {
    replace_pseudo_function(program->function);
}


// PASS 2

asm_instruction_node* asm_create_stackalloc_instr(int size) {
    asm_stackalloc_node* node = MALLOC(asm_stackalloc_node);
    node->n_bytes = size;

    asm_instruction_node* i = MALLOC(asm_instruction_node);
    i->type = INSTR_STACKALLOC;

    return i;
}

void add_stackalloc_function_cleanup_mov_operands_function(asm_function_node* function) {
    if (function->sm == NULL) {
        printf("stackmap not initialized...\n");
        exit(1);
    }

    list(asm_instruction_node*)* instrs = function->instructions;

    // insert the stackalloc instruction at the beginning of the function
    asm_instruction_node* stackalloc_instr = asm_create_stackalloc_instr(stackmap_size(function->sm));
    list_add(instrs, (void*)stackalloc_instr, 0);

    asm_instruction_node* instr;
    // iterate through each instruction, replacing invalid stores with registers...
    for (int i = 0; i < instrs->len; i++) {
        //instr = (asm_instruction_node*)list_get(instrs, i);
        // TODO: turn (mov mem, mem) -> (mov mem, reg; mov reg, mem)
    }

}

void add_stackalloc_function_cleanup_mov_operands(asm_program_node* program) {
    add_stackalloc_function_cleanup_mov_operands_function(program->function);
}
