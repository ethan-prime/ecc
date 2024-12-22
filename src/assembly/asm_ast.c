#include "asm_ast.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

asm_immediate_node* asm_create_immediate(int n) {
    asm_immediate_node* node = MALLOC(asm_immediate_node);
    node->i = n;
    return node;
}

operand_node* asm_create_register_operand(asm_register_t reg) {
    asm_register_node* node = MALLOC(asm_register_node);
    node->reg = reg;
    
    operand_node* op = MALLOC(operand_node);
    op->type = REG;
    op->operand.reg = node;

    return op;
}

asm_pseudo_node* asm_create_pseudo(char* identifier) {
    asm_pseudo_node* node = MALLOC(asm_pseudo_node);
    node->reg_name = identifier;
    return node;
}

operand_node* ir_val_to_operand(ir_val_node* val) {
    operand_node* node = MALLOC(operand_node);
    if (val->type == IR_CONSTANT) {
        node->type = IMM;
        node->operand.imm = asm_create_immediate(val->val.constant->i);
    } else if (val->type == IR_VAR) {
        node->type = PSEUDO;
        node->operand.pseudo = asm_create_pseudo(val->val.var->identifier);
    }
    return node;
}
#define OPERAND(val) (ir_val_to_operand(val))

asm_i* asm_create_move_instr(operand_node* src, operand_node* dest) {
    asm_move_node* node = MALLOC(asm_move_node);
    node->src = src;
    node->dest = dest;

    asm_i* mov = MALLOC(asm_i);
    mov->type = INSTR_MOV;
    mov->instruction.mov = node;

    return mov;
}

asm_i* asm_create_unary_instr(asm_unary_op op, operand_node* dest) {
    asm_unary_node* node = MALLOC(asm_unary_node);
    node->op = op;
    node->dest = dest;

    asm_i* i = MALLOC(asm_i);
    i->type = INSTR_UNARY;
    i->instruction.unary = node;

    return i;
}

asm_i* asm_create_binary_instr(asm_binary_op op, operand_node* src, operand_node* dest) {
    asm_binary_node* node = MALLOC(asm_binary_node);
    node->op = op;
    node->src = src;
    node->dest = dest;

    asm_i* i = MALLOC(asm_i);
    i->type = INSTR_BINARY;
    i->instruction.binary = node;

    return i;
}

asm_i* asm_create_idiv_instr(operand_node* divisor) {
    asm_idiv_node* node = MALLOC(asm_idiv_node);
    node->divisor = divisor;

    asm_i* i = MALLOC(asm_i);
    i->type = INSTR_IDIV;
    i->instruction.idiv = node;

    return i;
}

asm_i* asm_create_ret_instr() {
    asm_i* instr = MALLOC(asm_i);
    instr->type = INSTR_RET;
    return instr;
}

asm_i* asm_create_cdq_instr() {
    asm_i* instr = MALLOC(asm_i);
    instr->type = INSTR_CDQ;
    return instr;
}

asm_unary_op ir_unary_op_to_asm_unary_op(ir_unary_op op) {
    if (op == IR_COMPLEMENT) {
        return ASM_NOT;
    } else {
        return ASM_NEG;
    }
}
#define ASM_UNARY_OP(op) (ir_unary_op_to_asm_unary_op(op))

list(asm_i*)* ir_unary_to_asm(ir_unary_node* unary) {
    list(asm_i*)* instrs = list_init();
    asm_i* mov = asm_create_move_instr(OPERAND(unary->src), OPERAND(unary->dest));
    asm_i* unary_ = asm_create_unary_instr(ASM_UNARY_OP(unary->op), OPERAND(unary->dest));

    list_append(instrs, (void*)mov);
    list_append(instrs, (void*)unary_);

    return instrs;
}

asm_binary_op ir_binary_op_to_asm_binary_op(ir_binary_op op) {
    if (op == IR_ADD) {
        return ASM_ADD;
    } else if (op == IR_SUBTRACT) {
        return ASM_SUB;
    } else {
        return ASM_MULT;
    }
}
#define ASM_BINARY_OP(op) (ir_binary_op_to_asm_binary_op(op))

list(asm_i*)* ir_div_to_asm(ir_binary_node* binary) {
    list(asm_i*)* instrs = list_init();
    asm_i* mov1 = asm_create_move_instr(OPERAND(binary->src1), REGISTER(AX));
    asm_i* cdq = asm_create_cdq_instr();
    asm_i* idiv = asm_create_idiv_instr(OPERAND(binary->src2));
    asm_i* mov2 = asm_create_move_instr(REGISTER(AX), OPERAND(binary->dest));

    list_append(instrs, (void*)mov1);
    list_append(instrs, (void*)cdq);
    list_append(instrs, (void*)idiv);
    list_append(instrs, (void*)mov2);

    return instrs;
}

list(asm_i*)* ir_remainder_to_asm(ir_binary_node* binary) {
    list(asm_i*)* instrs = list_init();
    asm_i* mov1 = asm_create_move_instr(OPERAND(binary->src1), REGISTER(AX));
    asm_i* cdq = asm_create_cdq_instr();
    asm_i* idiv = asm_create_idiv_instr(OPERAND(binary->src2));
    asm_i* mov2 = asm_create_move_instr(REGISTER(DX), OPERAND(binary->dest)); // one difference from div instruction!

    list_append(instrs, (void*)mov1);
    list_append(instrs, (void*)cdq);
    list_append(instrs, (void*)idiv);
    list_append(instrs, (void*)mov2);

    return instrs;
}

list(asm_i*)* ir_binary_to_asm(ir_binary_node* binary) {
    // divide instructions are weird in x86... we need 
    // to convert them differently.
    if (binary->op == IR_DIVIDE) {
        return ir_div_to_asm(binary);
    } else if (binary->op == IR_REMAINDER) {
        return ir_remainder_to_asm(binary);
    }

    // for add, mult, and sub:
    list(asm_i*)* instrs = list_init();
    asm_i* mov = asm_create_move_instr(OPERAND(binary->src1), OPERAND(binary->dest));
    asm_i* binary_ = asm_create_binary_instr(ASM_BINARY_OP(binary->op), OPERAND(binary->src2), OPERAND(binary->dest));

    list_append(instrs, (void*)mov);
    list_append(instrs, (void*)binary_);

    return instrs;
}

list(asm_i*)* ir_return_to_asm(ir_return_node* ret) {
    list(asm_i*)* instrs = list_init();
    asm_i* mov = asm_create_move_instr(OPERAND(ret->val), asm_create_register_operand(AX));
    asm_i* ret_ = asm_create_ret_instr();

    // add instructions to instructions list
    list_append(instrs, (void*)mov);
    list_append(instrs, (void*)ret_);

    return instrs;
}

asm_function_node* ir_function_to_asm(ir_function_node* function) {
    asm_function_node* node = MALLOC(asm_function_node);
    node->identifier = function->identifier;
    
    list(asm_i*)* instrs = list_init();
    for(int i = 0; i < function->instructions->len; i++) {
        ir_instruction_node* instr = (ir_instruction_node*)list_get(function->instructions, i);
        switch (instr->type) {
            case IR_INSTR_UNARY:
                list_concat(instrs, ir_unary_to_asm(instr->instruction.unary));
                break;
            case IR_INSTR_RET:
                list_concat(instrs, ir_return_to_asm(instr->instruction.ret));
                break;
            case IR_INSTR_BINARY:
                list_concat(instrs, ir_binary_to_asm(instr->instruction.binary));
                break;
        }
    }
    node->instructions = instrs;
    node->sm = NULL; // stackmap is initially null, we set this later...

    return node;
}

asm_program_node* ir_program_to_asm(ir_program_node* program) {
    asm_program_node* node = MALLOC(asm_program_node);

    node->function = ir_function_to_asm(program->function);

    return node;
}