#include "asm_ast.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

asm_immediate_node *asm_create_immediate(int n)
{
    asm_immediate_node *node = MALLOC(asm_immediate_node);
    node->i = n;
    return node;
}

operand_node *asm_create_register_operand(asm_register_t reg)
{
    asm_register_node *node = MALLOC(asm_register_node);
    node->reg = reg;

    operand_node *op = MALLOC(operand_node);
    op->type = REG;
    op->operand.reg = node;

    return op;
}
#define REGISTER(r) (asm_create_register_operand(r))

asm_pseudo_node *asm_create_pseudo(char *identifier)
{
    asm_pseudo_node *node = MALLOC(asm_pseudo_node);
    node->reg_name = identifier;
    return node;
}

operand_node *ir_val_to_operand(ir_val_node *val)
{
    operand_node *node = MALLOC(operand_node);
    if (val->type == IR_CONSTANT)
    {
        node->type = IMM;
        node->operand.imm = asm_create_immediate(val->val.constant->i);
    }
    else if (val->type == IR_VAR)
    {
        node->type = PSEUDO;
        node->operand.pseudo = asm_create_pseudo(val->val.var->identifier);
    }
    return node;
}
#define OPERAND(val) (ir_val_to_operand(val))

operand_node *asm_create_immediate_operand(int n)
{
    operand_node *operand = MALLOC(operand_node);
    operand->type = IMM;
    operand->operand.imm = asm_create_immediate(n);

    return operand;
}
#define IMMEDIATE(n) (asm_create_immediate_operand(n))

asm_i *asm_create_mov_instr(op_size size, operand_node *src, operand_node *dest)
{
    asm_move_node *node = MALLOC(asm_move_node);
    node->src = src;
    node->dest = dest;
    node->size = size;

    asm_i *mov = MALLOC(asm_i);
    mov->type = INSTR_MOV;
    mov->instruction.mov = node;

    return mov;
}

asm_i *asm_create_unary_instr(asm_unary_op op, operand_node *dest)
{
    asm_unary_node *node = MALLOC(asm_unary_node);
    node->op = op;
    node->dest = dest;

    asm_i *i = MALLOC(asm_i);
    i->type = INSTR_UNARY;
    i->instruction.unary = node;

    return i;
}

asm_i *asm_create_binary_instr(asm_binary_op op, operand_node *src, operand_node *dest)
{
    asm_binary_node *node = MALLOC(asm_binary_node);
    node->op = op;
    node->src = src;
    node->dest = dest;

    asm_i *i = MALLOC(asm_i);
    i->type = INSTR_BINARY;
    i->instruction.binary = node;

    return i;
}

asm_i *asm_create_idiv_instr(operand_node *divisor)
{
    asm_idiv_node *node = MALLOC(asm_idiv_node);
    node->divisor = divisor;

    asm_i *i = MALLOC(asm_i);
    i->type = INSTR_IDIV;
    i->instruction.idiv = node;

    return i;
}

asm_i *asm_create_ret_instr()
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_RET;
    return instr;
}

asm_i *asm_create_cdq_instr()
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_CDQ;
    return instr;
}

asm_i *asm_create_jmp_instr(char *identifier)
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_JMP;

    asm_jmp_node *node = MALLOC(asm_jmp_node);
    node->identifier = identifier;

    instr->instruction.jmp = node;

    return instr;
}

asm_i *asm_create_jmpcc_instr(cond_code type, char *identifier)
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_JMPCC;

    asm_jmpcc_node *node = MALLOC(asm_jmpcc_node);
    node->identifier = identifier;
    node->type = type;

    instr->instruction.jmpcc = node;

    return instr;
}

asm_i *asm_create_cmp_instr(operand_node *src1, operand_node *src2)
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_CMP;

    asm_cmp_node *node = MALLOC(asm_cmp_node);
    node->src1 = src1;
    node->src2 = src2;

    instr->instruction.cmp = node;

    return instr;
}

asm_i *asm_create_setcc_instr(cond_code type, operand_node *dest)
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_SETCC;

    asm_setcc_node *node = MALLOC(asm_setcc_node);
    node->dest = dest;
    node->type = type;

    instr->instruction.setcc = node;

    return instr;
}

asm_i *asm_create_label_instr(char *identifier)
{
    asm_i *instr = MALLOC(asm_i);
    instr->type = INSTR_LABEL;

    asm_label_node *node = MALLOC(asm_label_node);
    node->identifier = identifier;

    instr->instruction.label = node;

    return instr;
}

asm_unary_op ir_unary_op_to_asm_unary_op(ir_unary_op op)
{
    if (op == IR_COMPLEMENT)
    {
        return ASM_NOT;
    }
    else
    {
        return ASM_NEG;
    }
}
#define ASM_UNARY_OP(op) (ir_unary_op_to_asm_unary_op(op))

list(asm_i *) * ir_unary_to_asm(ir_unary_node *unary)
{
    if (unary->op == IR_NOT)
    {
        // this is equivalent to checking if x == 0, just return those instructions...
        ir_binary_node *binary = MALLOC(ir_binary_node);
        binary->src1 = unary->src;
        ir_val_node *src2 = MALLOC(ir_val_node);
        src2->type = IR_CONSTANT;
        ir_constant_node *constant = MALLOC(ir_constant_node);
        constant->i = 0;
        src2->val.constant = constant;
        binary->src2 = src2;
        binary->dest = unary->dest;
        binary->op = IR_LOGICAL_EQUAL;

        return ir_binary_to_asm(binary);
    }

    list(asm_i *) *instrs = list_init();
    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, OPERAND(unary->src), OPERAND(unary->dest));
    asm_i *unary_ = asm_create_unary_instr(ASM_UNARY_OP(unary->op), OPERAND(unary->dest));

    list_append(instrs, (void *)mov);
    list_append(instrs, (void *)unary_);

    return instrs;
}

asm_binary_op ir_binary_op_to_asm_binary_op(ir_binary_op op)
{
    if (op == IR_ADD)
    {
        return ASM_ADD;
    }
    else if (op == IR_SUBTRACT)
    {
        return ASM_SUB;
    }
    else if (op == IR_MULTIPLY)
    {
        return ASM_MULT;
    }
    else if (op == IR_BITWISE_AND)
    {
        return ASM_AND;
    }
    else if (op == IR_BITWISE_OR)
    {
        return ASM_OR;
    }
    else if (op == IR_BITWISE_XOR)
    {
        return ASM_XOR;
    }
    else if (op == IR_BITWISE_LEFT_SHIFT)
    {
        return ASM_LEFT_SHIFT;
    }
    else /*if (op == IR_BITWISE_RIGHT_SHIFT)*/
    {
        return ASM_RIGHT_SHIFT;
    }
}
#define ASM_BINARY_OP(op) (ir_binary_op_to_asm_binary_op(op))

list(asm_i *) * ir_div_to_asm(ir_binary_node *binary)
{
    list(asm_i *) *instrs = list_init();
    asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, OPERAND(binary->src1), REGISTER(AX));
    asm_i *cdq = asm_create_cdq_instr();
    asm_i *idiv = asm_create_idiv_instr(OPERAND(binary->src2));
    asm_i *mov2 = asm_create_mov_instr(OP_4_BYTES, REGISTER(AX), OPERAND(binary->dest));

    list_append(instrs, (void *)mov1);
    list_append(instrs, (void *)cdq);
    list_append(instrs, (void *)idiv);
    list_append(instrs, (void *)mov2);

    return instrs;
}

list(asm_i *) * ir_remainder_to_asm(ir_binary_node *binary)
{
    list(asm_i *) *instrs = list_init();
    asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, OPERAND(binary->src1), REGISTER(AX));
    asm_i *cdq = asm_create_cdq_instr();
    asm_i *idiv = asm_create_idiv_instr(OPERAND(binary->src2));
    asm_i *mov2 = asm_create_mov_instr(OP_4_BYTES, REGISTER(DX), OPERAND(binary->dest)); // one difference from div instruction!

    list_append(instrs, (void *)mov1);
    list_append(instrs, (void *)cdq);
    list_append(instrs, (void *)idiv);
    list_append(instrs, (void *)mov2);

    return instrs;
}

list(asm_i *) * ir_shift_to_asm(ir_binary_node *binary)
{
    list(asm_i *) *instrs = list_init();
    // move "src" operand to dest. so, for Binary(%1, %2, %3), this does movl %1, %3
    asm_i *mov1 = asm_create_mov_instr(OP_4_BYTES, OPERAND(binary->src1), OPERAND(binary->dest));
    // movb %2, %cl
    asm_i *mov2 = asm_create_mov_instr(OP_1_BYTE, OPERAND(binary->src2), REGISTER(CX));
    asm_i *binary_ = asm_create_binary_instr(ASM_BINARY_OP(binary->op), REGISTER(CX), OPERAND(binary->dest));

    list_append(instrs, (void *)mov1);
    list_append(instrs, (void *)mov2);
    list_append(instrs, (void *)binary_);

    return instrs;
}

int is_relational(ir_binary_op op)
{
    return (op == IR_LOGICAL_GT || op == IR_LOGICAL_LT || op == IR_LOGICAL_LTE || op == IR_LOGICAL_GTE || op == IR_LOGICAL_EQUAL || op == IR_LOGICAL_NOT_EQUAL);
}

cond_code ir_cond_code_to_asm(ir_binary_op op)
{
    if (op == IR_LOGICAL_GT)
    {
        return G;
    }
    else if (op == IR_LOGICAL_LT)
    {
        return L;
    }
    else if (op == IR_LOGICAL_LTE)
    {
        return LE;
    }
    else if (op == IR_LOGICAL_GTE)
    {
        return GE;
    }
    else if (op == IR_LOGICAL_EQUAL)
    {
        return E;
    }
    else if (op == IR_LOGICAL_NOT_EQUAL)
    {
        return NE;
    }
    return -1;
}
#define COND_CODE(c) (ir_cond_code_to_asm(c))

list(asm_i *) * ir_relational_to_asm(ir_binary_node *binary)
{
    list(asm_i *) *instrs = list_init();

    asm_i *cmp = asm_create_cmp_instr(OPERAND(binary->src2), OPERAND(binary->src1));
    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, IMMEDIATE(0), OPERAND(binary->dest));
    asm_i *setcc = asm_create_setcc_instr(COND_CODE(binary->op), OPERAND(binary->dest));

    list_append(instrs, (void *)cmp);
    list_append(instrs, (void *)mov);
    list_append(instrs, (void *)setcc);

    return instrs;
}

list(asm_i *) * ir_binary_to_asm(ir_binary_node *binary)
{
    // divide instructions are weird in x86... we need
    // to convert them differently.
    if (binary->op == IR_DIVIDE)
    {
        return ir_div_to_asm(binary);
    }
    else if (binary->op == IR_REMAINDER)
    {
        return ir_remainder_to_asm(binary);
    }
    else if (binary->op == IR_BITWISE_LEFT_SHIFT || binary->op == IR_BITWISE_RIGHT_SHIFT)
    {
        return ir_shift_to_asm(binary);
    }
    else if (is_relational(binary->op))
    {
        return ir_relational_to_asm(binary); // <, >, <=, >=, ==, !=
    }

    // for add, mult, and sub:
    list(asm_i *) *instrs = list_init();
    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, OPERAND(binary->src1), OPERAND(binary->dest));
    asm_i *binary_ = asm_create_binary_instr(ASM_BINARY_OP(binary->op), OPERAND(binary->src2), OPERAND(binary->dest));

    list_append(instrs, (void *)mov);
    list_append(instrs, (void *)binary_);

    return instrs;
}

list(asm_i *) * ir_return_to_asm(ir_return_node *ret)
{
    list(asm_i *) *instrs = list_init();
    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, OPERAND(ret->val), asm_create_register_operand(AX));
    asm_i *ret_ = asm_create_ret_instr();

    // add instructions to instructions list
    list_append(instrs, (void *)mov);
    list_append(instrs, (void *)ret_);

    return instrs;
}

list(asm_i *) * ir_jump_to_asm(ir_jump_node *jump)
{
    list(asm_i *) *instrs = list_init();

    asm_i *jmp = asm_create_jmp_instr(jump->target);
    list_append(instrs, (void *)jmp);

    return instrs;
}

list(asm_i *) * ir_jumpz_to_asm(ir_jumpz_node *jumpz)
{
    list(asm_i *) *instrs = list_init();

    asm_i *cmp = asm_create_cmp_instr(IMMEDIATE(0), OPERAND(jumpz->condition));
    asm_i *jmpcc = asm_create_jmpcc_instr(E, jumpz->target);

    list_append(instrs, (void *)cmp);
    list_append(instrs, (void *)jmpcc);

    return instrs;
}

list(asm_i *) * ir_jumpnz_to_asm(ir_jumpnz_node *jumpnz)
{
    list(asm_i *) *instrs = list_init();

    asm_i *cmp = asm_create_cmp_instr(IMMEDIATE(0), OPERAND(jumpnz->condition));
    asm_i *jmpcc = asm_create_jmpcc_instr(NE, jumpnz->target);

    list_append(instrs, (void *)cmp);
    list_append(instrs, (void *)jmpcc);

    return instrs;
}

list(asm_i *) * ir_copy_to_asm(ir_copy_node *copy)
{
    list(asm_i *) *instrs = list_init();

    asm_i *mov = asm_create_mov_instr(OP_4_BYTES, OPERAND(copy->src), OPERAND(copy->dest));
    list_append(instrs, (void *)mov);

    return instrs;
}

list(asm_i *) * ir_label_to_asm(ir_label_node *label)
{
    list(asm_i *) *instrs = list_init();

    asm_i *lbl = asm_create_label_instr(label->identifier);
    list_append(instrs, (void *)lbl);

    return instrs;
}

asm_function_node *ir_function_to_asm(ir_function_node *function)
{
    asm_function_node *node = MALLOC(asm_function_node);
    node->identifier = function->identifier;

    list(asm_i *) *instrs = list_init();
    for (int i = 0; i < function->instructions->len; i++)
    {
        ir_instruction_node *instr = (ir_instruction_node *)list_get(function->instructions, i);
        switch (instr->type)
        {
        case IR_INSTR_UNARY:
            list_concat(instrs, ir_unary_to_asm(instr->instruction.unary));
            break;
        case IR_INSTR_RET:
            list_concat(instrs, ir_return_to_asm(instr->instruction.ret));
            break;
        case IR_INSTR_BINARY:
            list_concat(instrs, ir_binary_to_asm(instr->instruction.binary));
            break;
        case IR_INSTR_COPY:
            list_concat(instrs, ir_copy_to_asm(instr->instruction.copy));
            break;
        case IR_INSTR_JUMP:
            list_concat(instrs, ir_jump_to_asm(instr->instruction.jump));
            break;
        case IR_INSTR_JUMPZ:
            list_concat(instrs, ir_jumpz_to_asm(instr->instruction.jumpz));
            break;
        case IR_INSTR_JUMPNZ:
            list_concat(instrs, ir_jumpnz_to_asm(instr->instruction.jumpnz));
            break;
        case IR_INSTR_LABEL:
            list_concat(instrs, ir_label_to_asm(instr->instruction.label));
            break;
        }
    }
    node->instructions = instrs;
    node->sm = NULL; // stackmap is initially null, we set this later...

    return node;
}

asm_program_node *ir_program_to_asm(ir_program_node *program)
{
    asm_program_node *node = MALLOC(asm_program_node);

    node->function = ir_function_to_asm(program->function);

    return node;
}