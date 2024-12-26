#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MALLOC(type) ((type *)malloc(sizeof(type)))

int IR_VAR_IDX = 0;
int IR_FALSE_IDX = 0;
int IR_END_IDX = 0;

// converts n to string repr, stores at *buf. MAKE SURE TO MALLOC floor(log10(n)) + 2 bytes (n_places + 1).
void int_to_str(char* buf, int n, int n_places) {
    for (int i = 0; i < n_places; i++) {
        buf[n_places-i-1] = 0x30 + (n % 10); // add the last digit to the buffer, convert to ascii
        n /= 10; // drop the last digit
    }
    buf[n_places] = '\0';
}

// makes an "n identifier": "<prefix><n>"
char* ir_make_n_ident(char* prefix, int n) {
    int n_places;
    if (n == 0) {
        n_places = 1;
    } else {
       n_places = (int)log10((double)n) + 1; 
    }
    size_t bytes = (size_t)(n_places + strlen(prefix) + 1);
    char* res = (char *)malloc(sizeof(char)*bytes);
    strcpy(res, prefix);
    int_to_str(&res[strlen(prefix)], n, n_places);
    return res;
}

char* ir_make_temp_var() {
    char* res = ir_make_n_ident("tmp.", IR_VAR_IDX);
    IR_VAR_IDX++;
    return res;
}   

char* ir_make_shortcircuit_label() {
    char* res = ir_make_n_ident("_sc.", IR_FALSE_IDX);
    IR_FALSE_IDX++;
    return res;
}

char* ir_make_end_label() {
    char* res = ir_make_n_ident("_end.", IR_END_IDX);
    IR_END_IDX++;
    return res;
}

ir_constant_node* ir_create_constant(int c) {
    ir_constant_node* node = MALLOC(ir_constant_node);
    node->i = c;
    return node;
}

ir_val_node* ir_create_const_val(int c) {
    ir_constant_node* constant = ir_create_constant(c);
    ir_val_node* val = MALLOC(ir_val_node);
    val->type = IR_CONSTANT;
    val->val.constant = constant;
    
    return val;
}

ir_val_node* ir_create_var(char* identifier) {
    ir_var_node* node = MALLOC(ir_var_node);
    node->identifier = identifier;
    ir_val_node* val = MALLOC(ir_val_node);
    val->type = IR_VAR;
    val->val.var = node;
    return val;
}

ir_return_node* ir_create_return_node(ir_val_node* val) {
    ir_return_node* node = MALLOC(ir_return_node);
    node->val = val;
    return node;
}

ir_instruction_node* ir_create_unary_instr(ir_unary_op op, ir_val_node* src, ir_val_node* dest) {
    ir_unary_node* node = MALLOC(ir_unary_node);
    node->op = op;
    node->src = src;
    node->dest = dest;

    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_UNARY;
    i->instruction.unary = node;

    return i;
}

ir_instruction_node* ir_create_binary_instr(ir_binary_op op, ir_val_node* src1, ir_val_node* src2, ir_val_node* dest) {
    ir_binary_node* node = MALLOC(ir_binary_node);
    node->op = op;
    node->src1 = src1;
    node->src2 = src2;
    node->dest = dest;

    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_BINARY;
    i->instruction.binary = node;

    return i;
}

ir_instruction_node* ir_create_jump_instr(char* target) {
    ir_jump_node* node = MALLOC(ir_jump_node);
    node->target = target;
    
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_JUMP;
    i->instruction.jump = node;
    return i;
}

ir_instruction_node* ir_create_jumpz_instr(ir_val_node* condition, char* target) {
    ir_jumpz_node* node = MALLOC(ir_jumpz_node);
    node->condition = condition;
    node->target = target;
    
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_JUMPZ;
    i->instruction.jumpz = node;
    return i;
}

ir_instruction_node* ir_create_jumpnz_instr(ir_val_node* condition, char* target) {
    ir_jumpnz_node* node = MALLOC(ir_jumpnz_node);
    node->condition = condition;
    node->target = target;
    
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_JUMPNZ;
    i->instruction.jumpnz = node;
    return i;
}

ir_instruction_node* ir_create_copy_instr(ir_val_node* src, ir_val_node* dest) {
    ir_copy_node* node = MALLOC(ir_copy_node);
    node->src = src;
    node->dest = dest;
    
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_COPY;
    i->instruction.copy = node;
    return i;
}

ir_instruction_node* ir_create_label_instr(char* identifier) {
    ir_label_node* node = MALLOC(ir_label_node);
    node->identifier = identifier;

    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_LABEL;
    i->instruction.label = node;
    return i;
}


ir_unary_op unary_op_to_ir(unary_op op) {
    if (op == COMPLEMENT) {
        return IR_COMPLEMENT;
    } else if (op == NEGATE) {
        return IR_NEGATE;
    } else {
        return IR_NOT;
    }
}

ir_binary_op binary_op_to_ir(binary_op op) {
    if (op == ADD || op == POSTFIX_INC || op == PREFIX_INC) {
        return IR_ADD;
    } else if (op == SUBTRACT || op == POSTFIX_DEC || op == PREFIX_DEC) {
        return IR_SUBTRACT;
    } else if (op == MULTIPLY) {
        return IR_MULTIPLY;
    } else if (op == DIVIDE) {
        return IR_DIVIDE;
    } else if (op == REMAINDER) {
        return IR_REMAINDER;
    } else if (op == BITWISE_AND) {
        return IR_BITWISE_AND;
    } else if (op == BITWISE_OR) {
        return IR_BITWISE_OR;
    } else if (op == BITWISE_XOR) {
        return IR_BITWISE_XOR;
    } else if (op == BITWISE_LEFT_SHIFT) {
        return IR_BITWISE_LEFT_SHIFT;
    } else if (op == BITWISE_RIGHT_SHIFT) {
        return IR_BITWISE_RIGHT_SHIFT;
    } else if (op == LOGICAL_LT) {
        return IR_LOGICAL_LT;
    } else if (op == LOGICAL_GT) {
        return IR_LOGICAL_GT;
    } else if (op == LOGICAL_LTE) {
        return IR_LOGICAL_LTE;
    } else if (op == LOGICAL_GTE) {
        return IR_LOGICAL_GTE;
    } else if (op == LOGICAL_EQUAL) {
        return IR_LOGICAL_EQUAL;
    } else if (op == LOGICAL_NOT_EQUAL) {
        return IR_LOGICAL_NOT_EQUAL;
    } else if (op == LOGICAL_AND) {
        return IR_LOGICAL_AND;
    } else {
        return IR_LOGICAL_OR;
    }
}

ir_val_node* variable_to_ir(variable_node* var) {
    return ir_create_var(var->identifier);
}

// <expr>++
// we need to return the before val.
ir_val_node* postfix_to_ir(assign_node* assign, list(ir_instruction_node *)* instructions) {
    // we know this is a "binary expression" with an lhs of a variable if this is a postfix conversion.
    // so, lets move the before value into a temp register and save that for later 
    ir_val_node* src = ir_create_var(assign->lvalue->expr.variable->identifier);
    ir_val_node* temp = ir_create_var(ir_make_temp_var());
    // save the before value to return for this expression
    ir_instruction_node* copy1 = ir_create_copy_instr(src, temp);

    // now we do the rest of the work: expr evaluation and variable assignmnet
    ir_val_node* res = expr_to_ir(assign->expr, instructions);
    ir_instruction_node* copy2 = ir_create_copy_instr(res, src); // src is the dest, here...

    list_append(instructions, (void*)copy1);
    list_append(instructions, (void*)copy2);

    return temp; // return the value before the increment that we saved
}

ir_val_node* assign_to_ir(assign_node* assign, list(ir_instruction_node *)* instructions) {
    // first, let's check if this assignment uses the postfix operator...
    if (assign->expr->type == EXPR_BINARY && (assign->expr->expr.binary_expr->op == POSTFIX_INC || assign->expr->expr.binary_expr->op == POSTFIX_DEC)) {
        return postfix_to_ir(assign, instructions);
    }
    ir_val_node* res = expr_to_ir(assign->expr, instructions);
    ir_val_node* dest = ir_create_var(assign->lvalue->expr.variable->identifier);
    ir_instruction_node* copy = ir_create_copy_instr(res, dest);
    list_append(instructions, (void*)copy);
    return dest;
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
        ir_instruction_node* unary_instr = ir_create_unary_instr(op, src, dest);
        list_append(instructions, (void*)unary_instr);
        // return destination val node so we can use the result.
        return dest;
    } else if (expr->type == EXPR_BINARY) {
        ir_binary_op op = binary_op_to_ir(expr->expr.binary_expr->op);
        if (op == IR_LOGICAL_AND || op == IR_LOGICAL_OR) {
            // short circuit operators...
            ir_val_node* v1 = expr_to_ir(expr->expr.binary_expr->lhs, instructions);
            // short circuit if zero for and, if nonzero for or.
            char* sc = ir_make_shortcircuit_label();
            ir_instruction_node* jmp1 = (op == IR_LOGICAL_AND) ? ir_create_jumpz_instr(v1, sc) : ir_create_jumpnz_instr(v1, sc);
            list_append(instructions, (void*)jmp1);
            ir_val_node* v2 = expr_to_ir(expr->expr.binary_expr->rhs, instructions);
            ir_instruction_node* jmp2 = (op == IR_LOGICAL_AND) ? ir_create_jumpz_instr(v2, sc) : ir_create_jumpnz_instr(v2, sc);
            list_append(instructions, (void*)jmp2);
            ir_val_node* val1 = MALLOC(ir_val_node);
            val1->type = IR_CONSTANT;
            // based off if we short circuit an && or ||.
            val1->val.constant = (op == IR_LOGICAL_AND) ? ir_create_constant(1) : ir_create_constant(0);
            char* dest_name = ir_make_temp_var();
            ir_val_node* dest = ir_create_var(dest_name);
            ir_instruction_node* copy1 = ir_create_copy_instr(val1, dest);
            list_append(instructions, (void*)copy1);
            char* end = ir_make_end_label();
            ir_instruction_node* jmp3 = ir_create_jump_instr(end);
            list_append(instructions, (void*)jmp3);
            ir_instruction_node* label1 = ir_create_label_instr(sc);
            list_append(instructions, (void*)label1);
            ir_val_node* val2 = MALLOC(ir_val_node);
            val2->type = IR_CONSTANT;
            // based off if we short circuit an && or ||.
            val2->val.constant = (op == IR_LOGICAL_AND) ? ir_create_constant(0) : ir_create_constant(1);
            ir_instruction_node* copy2 = ir_create_copy_instr(val2, dest);
            list_append(instructions, (void*)copy2);
            ir_instruction_node* label2 = ir_create_label_instr(end);
            list_append(instructions, (void*)label2);
            return dest;
        }
        // else, not short circuit operator:
        ir_val_node* src1 = expr_to_ir(expr->expr.binary_expr->lhs, instructions);
        ir_val_node* src2 = expr_to_ir(expr->expr.binary_expr->rhs, instructions);
        char* dest_name = ir_make_temp_var();
        ir_val_node* dest = ir_create_var(dest_name);

        ir_instruction_node* binary_instr = ir_create_binary_instr(op, src1, src2, dest);\

        // add binary expression to instructions
        list_append(instructions, (void*)binary_instr);

        return dest;
    } else if (expr->type == EXPR_VARIABLE) {
        return variable_to_ir(expr->expr.variable);
    } else if (expr->type == EXPR_ASSIGN) {
        ir_val_node* res = assign_to_ir(expr->expr.assign, instructions);
        return res;
    }
    // invalid instruction, unreachable...
    return NULL;
}

// STATEMENTS:
void return_to_ir(return_node *ret, list(ir_instruction_node*)* instrs) {
    ir_val_node* res = expr_to_ir(ret->expr, instrs);
    ir_return_node* ret_node = ir_create_return_node(res);
    ir_instruction_node* ret_instr = MALLOC(ir_instruction_node);
    ret_instr->type = IR_INSTR_RET;
    ret_instr->instruction.ret = ret_node;
    list_append(instrs, (void*)ret_instr);
}

void add_return_0(list(ir_instruction_node*)* instrs) {
    ir_return_node* ret_node = ir_create_return_node(ir_create_const_val(0));
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_RET;
    i->instruction.ret = ret_node;
    list_append(instrs, (void*)i);
}

list(ir_instruction_node *)* statement_to_ir(statement_node *stmt) {
    // right now, we only have return statements
    list(ir_instruction_node*)* instrs = list_init();
    if (stmt->type == STMT_RET) {
        return_to_ir(stmt->stmt.ret, instrs);
    } else if (stmt->type == STMT_EXPR) {
        expr_to_ir(stmt->stmt.expr, instrs);
    } // null stmt has no instrs...
    return instrs;
}

list(ir_instruction_node*)* declaration_to_ir(declaration_node* declare) {
    list(ir_instruction_node*)* instrs = list_init();
    if (declare->init != NULL) {
        // handle this like an assignment
        ir_val_node* res = expr_to_ir(declare->init, instrs);
        ir_val_node* dest = ir_create_var(declare->identifier);
        ir_instruction_node* copy = ir_create_copy_instr(res, dest);
        list_append(instrs, (void*)copy);
    }
    return instrs;
}

list(ir_instruction_node*)* block_item_to_ir(block_item_node* block_item) {
    if (block_item->type == BLOCK_DECLARE) {
        return declaration_to_ir(block_item->item.declare);
    } else {
        return statement_to_ir(block_item->item.stmt);
    }
}

ir_function_node* function_to_ir(function_node *function) {
    ir_function_node* node = MALLOC(ir_function_node);
    node->identifier = function->identifier;
    
    node->instructions = list_init(); // for function instructions
    
    for (int i = 0; i < function->body->len; i++) {
        block_item_node* block_item = (block_item_node*)list_get(function->body, i);
        list_concat(node->instructions, block_item_to_ir(block_item)); // add instrs to body...
    }

    add_return_0(node->instructions); // in case the function forgets a return value, return 0. c standard

    return node;
}

ir_program_node* program_to_ir(program_node *program) {
    ir_program_node* node = MALLOC(ir_program_node);

    node->function = function_to_ir(program->function);

    return node;
}

// prints TAC AST.
void ir_print_program(ir_program_node* program_ir);