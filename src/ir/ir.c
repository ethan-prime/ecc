#include "../config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define MALLOC(type) ((type *)malloc(sizeof(type)))

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

#if IR_ENABLE

#include "ir.h"
#include <stdio.h>

int IR_VAR_IDX = 0;
int IR_FALSE_IDX = 0;
int IR_END_IDX = 0;
int IR_ELSE_IDX = 0;

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

char* ir_make_else_label() {
    char* res = ir_make_n_ident("_else.", IR_ELSE_IDX);
    IR_ELSE_IDX++;
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

ir_instruction_node* ir_create_function_call_instr(char* function_name, list(ir_val_node*)* args, ir_val_node* dest) {
    ir_function_call_node* node = MALLOC(ir_function_call_node);
    node->function_name = function_name;
    node->args = args;
    node->dest = dest;

    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_FUNCTION_CALL;
    i->instruction.function_call = node;
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

ir_val_node* ternary_to_ir(ternary_node* ternary, list(ir_instruction_node*)* instrs) {
    ir_val_node* c = expr_to_ir(ternary->condition, instrs);
    ir_val_node* res = ir_create_var(ir_make_temp_var());
    char* e2_label = ir_make_else_label();
    ir_instruction_node* jmp1 = ir_create_jumpz_instr(c, e2_label);
    list_append(instrs, (void*)jmp1);
    ir_val_node* v1 = expr_to_ir(ternary->condition_true, instrs);
    ir_instruction_node* copy1 = ir_create_copy_instr(v1, res);
    list_append(instrs, (void*)copy1);
    char* end_label = ir_make_end_label();
    ir_instruction_node* jmp2 = ir_create_jump_instr(end_label);
    list_append(instrs, (void*)jmp2);
    ir_instruction_node* e2_label_instr = ir_create_label_instr(e2_label);
    list_append(instrs, (void*)e2_label_instr);
    ir_val_node* v2 = expr_to_ir(ternary->condition_false, instrs);
    ir_instruction_node* copy2 = ir_create_copy_instr(v2, res);
    list_append(instrs, (void*)copy2);
    ir_instruction_node* end_label_instr = ir_create_label_instr(end_label);
    list_append(instrs, (void*)end_label_instr);

    return res;
}

ir_val_node* function_call_to_ir(function_call_node* call, list(ir_instruction_node*)* instrs) {
    // evaluate all the args
    list(ir_val_node*)* arg_list = list_init();
    for (int i = 0; i < call->args->len; i++) {
        expr_node* arg = (expr_node*)list_get(call->args, i);
        ir_val_node* res = expr_to_ir(arg, instrs);
        list_append(arg_list, (void*)res);
    }

    ir_val_node* call_result = ir_create_var(ir_make_temp_var());
    ir_instruction_node* func_call = ir_create_function_call_instr(call->identifier, arg_list, call_result);
    list_append(instrs, (void*)func_call);
    return call_result;
}

// appends instructions to generate expr -> ir, returns ir_val_node with DEST ir val.
ir_val_node* expr_to_ir(expr_node* expr, list(ir_instruction_node *)* instructions) {
    if (expr == NULL) {
        return NULL;
    }
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
        return assign_to_ir(expr->expr.assign, instructions);
    } else if (expr->type == EXPR_TERNARY) {
        return ternary_to_ir(expr->expr.ternary, instructions);
    } else if (expr->type == EXPR_FUNCTION_CALL) {
        return function_call_to_ir(expr->expr.function_call, instructions);
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

void if_to_ir(if_node* if_stmt, list(ir_instruction_node*)* instrs) {
    ir_val_node* c = expr_to_ir(if_stmt->condition, instrs);
    char* end_label = ir_make_end_label();
    if (if_stmt->else_stmt == NULL) {
        // add instrs for evaluating condition
        ir_instruction_node* jmp1 = ir_create_jumpz_instr(c, end_label);
        list_append(instrs, (void*)jmp1);
        list(ir_instruction_node*)* stmt = statement_to_ir(if_stmt->then_stmt);
        // add instrs for then stmt
        list_concat(instrs, stmt);
    } else {
        // we have an else statement
        char* else_label = ir_make_else_label();
        ir_instruction_node* jmp2 = ir_create_jumpz_instr(c, else_label);
        list_append(instrs, (void*)jmp2);

        list(ir_instruction_node*)* stmt = statement_to_ir(if_stmt->then_stmt);
        list_concat(instrs, stmt);

        // jump to end
        ir_instruction_node* jmp1 = ir_create_jump_instr(end_label);
        list_append(instrs, (void*)jmp1);

        ir_instruction_node* else_label_instr = ir_create_label_instr(else_label);
        list_append(instrs, (void*)else_label_instr);

        list(ir_instruction_node*)* else_stmt = statement_to_ir(if_stmt->else_stmt);
        list_concat(instrs, else_stmt);
    }
    ir_instruction_node* end = ir_create_label_instr(end_label);
    list_append(instrs, (void*)end);
}

void add_return_0(list(ir_instruction_node*)* instrs) {
    ir_return_node* ret_node = ir_create_return_node(ir_create_const_val(0));
    ir_instruction_node* i = MALLOC(ir_instruction_node);
    i->type = IR_INSTR_RET;
    i->instruction.ret = ret_node;
    list_append(instrs, (void*)i);
}

// returns prefix+loop_label str.
char* make_prefix_loop_label(char* prefix, char* loop_label) {
    char* break_label = (char*)malloc(sizeof(char)*(strlen(loop_label)+strlen(prefix)+1));
    strcpy(break_label, prefix);
    strcat(break_label, loop_label);
    return break_label;
}

void break_to_ir(break_node* break_node, list(ir_instruction_node*)* instrs) {    
    ir_instruction_node* i = ir_create_jump_instr(make_prefix_loop_label("_break", break_node->loop_label));
    list_append(instrs, (void*)i);
}

void continue_to_ir(continue_node* continue_node, list(ir_instruction_node*)* instrs) {
    ir_instruction_node* i = ir_create_jump_instr(make_prefix_loop_label("_continue", continue_node->loop_label));
    list_append(instrs, (void*)i);
}

void do_while_to_ir(do_while_node* do_while_node, list(ir_instruction_node*)* instrs) {
    char* start_name = make_prefix_loop_label("_start", do_while_node->loop_label);
    ir_instruction_node* start_label = ir_create_label_instr(start_name);
    list_append(instrs, (void*)start_label);
    list(ir_instruction_node*)* body = statement_to_ir(do_while_node->body);
    list_concat(instrs, body);
    char* continue_name = make_prefix_loop_label("_continue", do_while_node->loop_label);
    ir_instruction_node* continue_label = ir_create_label_instr(continue_name);
    list_append(instrs, (void*)continue_label);
    ir_val_node* c = expr_to_ir(do_while_node->condition, instrs);
    ir_instruction_node* jmp1 = ir_create_jumpnz_instr(c, start_name);
    list_append(instrs, (void*)jmp1);
    char* break_name = make_prefix_loop_label("_break", do_while_node->loop_label);
    ir_instruction_node* break_label = ir_create_label_instr(break_name);
    list_append(instrs, (void*)break_label);
}

void while_to_ir(while_node* while_node, list(ir_instruction_node*)* instrs) {
    char* continue_name = make_prefix_loop_label("_continue", while_node->loop_label);
    ir_instruction_node* continue_label = ir_create_label_instr(continue_name);
    list_append(instrs, (void*)continue_label);
    ir_val_node* c = expr_to_ir(while_node->condition, instrs);
    char* break_name = make_prefix_loop_label("_break", while_node->loop_label);
    ir_instruction_node* jmp1 = ir_create_jumpz_instr(c, break_name);
    list_append(instrs, (void*)jmp1);
    list(ir_instruction_node*)* body = statement_to_ir(while_node->body);
    list_concat(instrs, body);
    ir_instruction_node* jmp2 = ir_create_jump_instr(continue_name);
    list_append(instrs, (void*)jmp2);
    ir_instruction_node* break_label = ir_create_label_instr(break_name);
    list_append(instrs, (void*)break_label);
}

void for_init_to_ir(for_init_node* for_init, list(ir_instruction_node*)* instrs) {
    if (for_init->type == INIT_DECL) {
        list_concat(instrs, variable_declaration_to_ir(for_init->for_init.init_declare));
    } else {
        expr_to_ir(for_init->for_init.init_expr, instrs);
    }
}

void for_to_ir(for_node* for_stmt, list(ir_instruction_node*)* instrs) {
    if (for_stmt->init != NULL) {
        for_init_to_ir(for_stmt->init, instrs);
    }

    char* start_name = make_prefix_loop_label("_start", for_stmt->loop_label);
    char* break_name = make_prefix_loop_label("_break", for_stmt->loop_label);
    char* continue_name = make_prefix_loop_label("_continue", for_stmt->loop_label);

    ir_instruction_node* start_label = ir_create_label_instr(start_name);
    list_append(instrs, (void*)start_label);

    if (for_stmt->condition != NULL) {
        ir_val_node* c = expr_to_ir(for_stmt->condition, instrs);
        ir_instruction_node* jmp1 = ir_create_jumpz_instr(c, break_name);
        list_append(instrs, (void*)jmp1);
    }

    list(ir_instruction_node*)* body = statement_to_ir(for_stmt->body);
    list_concat(instrs, body);

    ir_instruction_node* continue_label = ir_create_label_instr(continue_name);
    list_append(instrs, (void*)continue_label);

    if (for_stmt->final_expr != NULL) {
        expr_to_ir(for_stmt->final_expr, instrs);
    }

    ir_instruction_node* jmp2 = ir_create_jump_instr(start_name);
    list_append(instrs, (void*)jmp2);

    ir_instruction_node* break_label = ir_create_label_instr(break_name);
    list_append(instrs, (void*)break_label);
}

list(ir_instruction_node *)* statement_to_ir(statement_node *stmt) {
    // right now, we only have return statements
    list(ir_instruction_node*)* instrs = list_init();
    if (stmt->type == STMT_RET) {
        return_to_ir(stmt->stmt.ret, instrs);
    } else if (stmt->type == STMT_EXPR) {
        expr_to_ir(stmt->stmt.expr, instrs);
    } else if (stmt->type == STMT_IF) {
        if_to_ir(stmt->stmt.if_stmt, instrs);
    } else if (stmt->type == STMT_COMPOUND) {
        instrs = block_to_ir(stmt->stmt.compound->block);
    } else if (stmt->type == STMT_DO_WHILE) {
        do_while_to_ir(stmt->stmt.do_while_node, instrs);
    } else if (stmt->type == STMT_WHILE) {
        while_to_ir(stmt->stmt.while_node, instrs);
    } else if (stmt->type == STMT_FOR) {
        for_to_ir(stmt->stmt.for_node, instrs);
    } else if (stmt->type == STMT_BREAK) {
        break_to_ir(stmt->stmt.break_node, instrs);
    } else if (stmt->type == STMT_CONTINUE) {
        continue_to_ir(stmt->stmt.continue_node, instrs);
    }
    // null stmt has no instrs...
    return instrs;
}

list(ir_instruction_node*)* variable_declaration_to_ir(variable_declaration_node* declare) {
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

list(ir_param_node*)* params_to_ir(list(param_node*)* params) {
    list(ir_param_node*)* ir_params = list_init();
    for (int i = 0; i < params->len; i++) {
        ir_param_node* ir_param = MALLOC(ir_param_node);
        ir_param->identifier = ((param_node*)list_get(params, i))->identifier;
        list_append(ir_params, (void*)ir_param);
    }
    return ir_params;
}

ir_function_node* function_declaration_to_ir(function_declaration_node* declare) {
    if (declare->body == NULL) {
        return NULL;
    }
    ir_function_node* node = MALLOC(ir_function_node);
    node->identifier = declare->name;
    node->params = params_to_ir(declare->params);
    node->instructions = block_to_ir(declare->body);
    return node;
}

list(ir_instruction_node*)* declaration_to_ir(declaration_node* declare) {
    if (declare->type == DECLARE_FUNCTION) {
        return NULL;
    } else {
        return variable_declaration_to_ir(declare->declaration.variable);
    } 
    //definitions will always be top-level, so we dont need to worry about reaching 
    // a function declaration here because we process those before everything else
}

list(ir_instruction_node*)* block_item_to_ir(block_item_node* block_item) {
    if (block_item->type == BLOCK_DECLARE) {
        return declaration_to_ir(block_item->item.declare);
    } else {
        return statement_to_ir(block_item->item.stmt);
    }
}

list(ir_instruction_node*)* block_to_ir(block_node* block) {
    list(ir_instruction_node*)* instrs = list_init();

    for (int i = 0; i < block->items->len; i++) {
        block_item_node* block_item = (block_item_node*)list_get(block->items, i);
        list(ir_instruction_node*)* new_instrs = block_item_to_ir(block_item);
        if (new_instrs != NULL) {
            list_concat(instrs, new_instrs); // add instrs to body...
        }
    }

    return instrs;
}

ir_program_node* program_to_ir(program_node *program) {
    ir_program_node* node = MALLOC(ir_program_node);

    node->functions = list_init();

    for (int i = 0; i < program->functions->len; i++) {
        function_declaration_node* func = (function_declaration_node*)list_get(program->functions, i);
        ir_function_node* ir_func = function_declaration_to_ir(func);
        // remember, we ignore any function without a body.
        if (ir_func != NULL) {
            add_return_0(ir_func->instructions);
            list_append(node->functions, (void*)ir_func);
        }
    }

    return node;
}

// prints TAC AST.
void ir_print_program(ir_program_node* program_ir);
#endif