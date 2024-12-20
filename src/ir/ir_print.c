#include "ir.h"

void ir_print_val(ir_val_node* val) {
    if (val->type == IR_CONSTANT) {
        printf("Constant(%d)", val->val.constant->i);
    } else if (val->type == IR_VAR) {
        printf("Var(%s)", val->val.var->identifier);
    }
}

void ir_print_return(ir_return_node *ret) {
    printf("Return(");
    ir_print_val(ret->val);
    printf(")\n");
}

void ir_print_unary(ir_unary_node* unary) {
    printf("Unary(");
    if (unary->op == IR_COMPLEMENT) {
        printf("Complement, ");
    } else if (unary->op == IR_NEGATE) {
        printf("Negate, ");
    }
    ir_print_val(unary->src);
    printf(", ");
    ir_print_val(unary->dest);
    printf(")\n");
}

void ir_print_instructions(list(ir_instruction_node)* instrs) {
    for(int i = 0; i < instrs->len; i++) {
        ir_instruction_node* instr = (ir_instruction_node*)list_get(instrs, i);
        if (instr->type == IR_INSTR_RET) {
            ir_print_return(instr->instruction.ret);
        } else if (instr->type == IR_INSTR_UNARY) {
            ir_print_unary(instr->instruction.unary);
        }
    }
}

void ir_print_function(ir_function_node* func) {
    printf("FUNCTION %s:\n", func->identifier);
    ir_print_instructions(func->instructions);
}

// pretty-prints the ast of the program.
void ir_print_program(ir_program_node *program) {
    ir_print_function(program->function);
}