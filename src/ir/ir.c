#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MALLOC(type) ((type *)malloc(sizeof(type)))

int IR_VAR_IDX = 0;

char* ir_make_temp_var() {
    int n_places;
    if (IR_VAR_IDX == 0) {
        n_places = 1;
    } else {
       n_places = (int)log10((double)IR_VAR_IDX) + 1; 
    }
    char* res = (char *)malloc(sizeof(char)*(n_places+2)); // space for % and \0
    res[0] = '%';
    int n = IR_VAR_IDX;
    for (int i = 0; i < n_places; i++) {
        res[n_places-i] = 0x30 + (n % 10); // add the last digit to the buffer, convert to ascii
        n /= 10; // drop the last digit
    }
    res[n_places+1] = '\0';
    IR_VAR_IDX++;
    return res;
}   

ir_constant_node* ir_create_constant(int c) {
    ir_constant_node* node = MALLOC(ir_constant_node);
    node->i = c;
    return node;
}

ir_val_node* ir_create_var(char* identifier) {
    ir_var_node* node = MALLOC(ir_var_node);
    node->identifier = identifier;
    ir_val_node* val = MALLOC(ir_val_node);
    val->type = IR_VAR;
    val->val.var = node;
    return val;
}

ir_unary_node* ir_create_unary_node(ir_unary_op op, ir_val_node* src, ir_val_node* dest) {
    ir_unary_node* node = MALLOC(ir_unary_node);
    node->op = op;
    node->src = src;
    node->dest = dest;
    return node;
}

ir_binary_node* ir_create_binary_node(ir_binary_op op, ir_val_node* src1, ir_val_node* src2, ir_val_node* dest) {
    ir_binary_node* node = MALLOC(ir_binary_node);
    node->op = op;
    node->src1 = src1;
    node->src2 = src2;
    node->dest = dest;
    return node;
}

ir_return_node* ir_create_return_node(ir_val_node* val) {
    ir_return_node* node = MALLOC(ir_return_node);
    node->val = val;
    return node;
}

ir_unary_op unary_op_to_ir(unary_op op) {
    if (op == COMPLEMENT) {
        return IR_COMPLEMENT;
    } else {
        return IR_NEGATE;
    }
}

ir_binary_op binary_op_to_ir(binary_op op) {
    if (op == ADD) {
        return IR_ADD;
    } else if (op == SUBTRACT) {
        return IR_SUBTRACT;
    } else if (op == MULTIPLY) {
        return IR_MULTIPLY;
    } else if (op == DIVIDE) {
        return IR_DIVIDE;
    } else {
        return IR_REMAINDER;
    }
}

// appends instructions to generate expr -> ir, returns ir_val_node with DEST ir val.
ir_val_node* expr_to_ir(expr_node* expr, list(ir_instruction_node *)* instructions) {
    if (expr->type == EXPR_CONSTANT) {
        ir_val_node* val = MALLOC(ir_val_node);
        val->type = IR_CONSTANT;
        val->val.constant = ir_create_constant(expr->expr.constant->val);
        return val;
    } else if (expr->type == EXPR_UNARY) {
        // process inner expr, add instructions to expression. get src name
        ir_val_node* src = expr_to_ir(expr->expr.unary_expr->expr, instructions);
        // create new dest variable.
        char* dest_name = ir_make_temp_var();
        ir_val_node* dest = ir_create_var(dest_name);
        // create new Unary TAC instruction.
        ir_unary_op op = unary_op_to_ir(expr->expr.unary_expr->op);
        ir_instruction_node* unary_instr = MALLOC(ir_instruction_node);
        unary_instr->type = IR_INSTR_UNARY;
        unary_instr->instruction.unary = ir_create_unary_node(op, src, dest);
        // add instructions to the list...
        list_append(instructions, (void*)unary_instr);
        // return destination val node so we can use the result.
        return dest;
    } else if (expr->type == EXPR_BINARY) {
        ir_val_node* src1 = expr_to_ir(expr->expr.binary_expr->lhs, instructions);
        ir_val_node* src2 = expr_to_ir(expr->expr.binary_expr->rhs, instructions);
        char* dest_name = ir_make_temp_var();
        ir_val_node* dest = ir_create_var(dest_name);
        ir_binary_op op = binary_op_to_ir(expr->expr.binary_expr->op);

        ir_instruction_node* binary_instr = MALLOC(ir_instruction_node);
        binary_instr->type = IR_INSTR_BINARY;
        binary_instr->instruction.binary = ir_create_binary_node(op, src1, src2, dest);

        // add binary expression to instructions
        list_append(instructions, (void*)binary_instr);

        return dest;
    }
    // invalid instruction, unreachable...
    return NULL;
}

// STATEMENTS:
list(ir_instruction_node *)* return_to_ir(return_node *ret) {
    list(ir_instruction_node*)* instrs = list_init();
    ir_val_node* res = expr_to_ir(ret->expr, instrs);
    ir_return_node* ret_node = ir_create_return_node(res);
    ir_instruction_node* ret_instr = MALLOC(ir_instruction_node);
    ret_instr->type = IR_INSTR_RET;
    ret_instr->instruction.ret = ret_node;
    list_append(instrs, (void*)ret_instr);
    return instrs;
}

list(ir_instruction_node *)* statement_to_ir(statement_node *stmt) {
    // right now, we only have return statements
    return return_to_ir(stmt->ret);
}

ir_function_node* function_to_ir(function_node *function) {
    ir_function_node* node = MALLOC(ir_function_node);

    node->identifier = function->identifier;
    node->instructions = statement_to_ir(function->body);

    return node;
}

ir_program_node* program_to_ir(program_node *program) {
    ir_program_node* node = MALLOC(ir_program_node);

    node->function = function_to_ir(program->function);

    return node;
}

// prints TAC AST.
void ir_print_program(ir_program_node* program_ir);