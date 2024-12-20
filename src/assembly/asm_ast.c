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

asm_instruction_node* asm_create_move_instr(operand_node* src, operand_node* dest) {
    asm_move_node* node = MALLOC(asm_move_node);
    node->src = src;
    node->dest = dest;

    asm_instruction_node* mov = MALLOC(asm_instruction_node);
    mov->type = INSTR_MOV;
    mov->instruction.mov = node;

    return mov;
}

asm_instruction_node* asm_create_unary_instr(asm_unary_op op, operand_node* dest) {
    asm_unary_node* node = MALLOC(asm_unary_node);
    node->op = op;
    node->dest = dest;

    asm_instruction_node* i = MALLOC(asm_instruction_node);
    i->type = INSTR_UNARY;
    i->instruction.unary = node;

    return i;
}

asm_instruction_node* asm_create_ret_instr() {
    asm_instruction_node* instr = MALLOC(asm_instruction_node);
    instr->type = INSTR_RET;
    return instr;
}

list(asm_instruction_node*)* ir_unary_to_asm(ir_unary_node* unary) {
    list(asm_instruction_node*)* instrs = list_init();
    asm_instruction_node* mov = asm_create_move_instr(ir_val_to_operand(unary->src), ir_val_to_operand(unary->dest));
    asm_instruction_node* unary_;
    if (unary->op == IR_COMPLEMENT) {
        unary_ = asm_create_unary_instr(ASM_NOT, ir_val_to_operand(unary->dest));
    } else {
        unary_ = asm_create_unary_instr(ASM_NEG, ir_val_to_operand(unary->dest));
    }

    list_append(instrs, (void*)mov);
    list_append(instrs, (void*)unary_);

    return instrs;
}

list(asm_instruction_node*)* ir_return_to_asm(ir_return_node* ret) {
    list(asm_instruction_node*)* instrs = list_init();
    asm_instruction_node* mov = asm_create_move_instr(ir_val_to_operand(ret->val), asm_create_register_operand(AX));
    asm_instruction_node* ret_ = asm_create_ret_instr();

    // add instructions to instructions list
    list_append(instrs, (void*)mov);
    list_append(instrs, (void*)ret_);

    return instrs;
}

asm_function_node* ir_function_to_asm(ir_function_node* function) {
    asm_function_node* node = MALLOC(asm_function_node);
    node->identifier = function->identifier;
    
    list(asm_instruction_node*)* instrs = list_init();
    for(int i = 0; i < function->instructions->len; i++) {
        ir_instruction_node* instr = (ir_instruction_node*)list_get(function->instructions, i);
        switch (instr->type) {
            case IR_INSTR_UNARY:
                list_concat(instrs, ir_unary_to_asm(instr->instruction.unary));
                break;
            case IR_INSTR_RET:
                list_concat(instrs, ir_return_to_asm(instr->instruction.ret));
                break;
        }
    }
    node->instructions = instrs;

    return node;
}

asm_program_node* ir_program_to_asm(ir_program_node* program) {
    asm_program_node* node = MALLOC(asm_program_node);

    node->function = ir_function_to_asm(program->function);

    return node;
}