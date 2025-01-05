#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void type_error(symboltable* symbols, char* message) {
    printf("Type Error: %s\n", message);
    symboltable_print(symbols);
    exit(1);
}

void typecheck_expr(symboltable* symbols, expr_node* expr) {
    if (expr->type == EXPR_FUNCTION_CALL) {
        symboltable_node* entry = symboltable_get(symbols, expr->expr.function_call->identifier);
        if (entry == NULL) {
            char err_message[256];
            sprintf(err_message, "Unknown function %s", expr->expr.function_call->identifier);
            type_error(symbols, err_message);
        }
        if (entry->type_spec->type == TYPE_INT) {
            char err_message[256];
            sprintf(err_message, "Variable %s used as function name", expr->expr.function_call->identifier);
            type_error(symbols, err_message);
        }
        if (entry->type_spec->param_count != expr->expr.function_call->args->len) {
            char err_message[256];
            sprintf(err_message, "Function %s called with wrong number of arguments", expr->expr.function_call->identifier);
            type_error(symbols, err_message);
        }
        for (int i = 0; i < expr->expr.function_call->args->len; i++) {
            expr_node* arg = (expr_node *)list_get(expr->expr.function_call->args, i);
            typecheck_expr(symbols, arg);
        }
    } else if (expr->type == EXPR_VARIABLE) {
        symboltable_node* entry = symboltable_get(symbols, expr->expr.variable->identifier);
        if (entry == NULL) {
            char err_message[256];
            sprintf(err_message, "Unknown function %s", expr->expr.variable->identifier);
            type_error(symbols, err_message);
        }
        if (entry->type_spec->type != TYPE_INT) {
            char err_message[256];
            sprintf(err_message, "Function %s used as a variable", expr->expr.variable->identifier);
            type_error(symbols, err_message);
        }
    } else if (expr->type == EXPR_UNARY) {
        typecheck_expr(symbols, expr->expr.unary_expr->expr);
    } else if (expr->type == EXPR_BINARY) {
        typecheck_expr(symbols, expr->expr.binary_expr->lhs);
        typecheck_expr(symbols, expr->expr.binary_expr->rhs);
    } else if (expr->type == EXPR_ASSIGN) {
        typecheck_expr(symbols, expr->expr.assign->lvalue);
        typecheck_expr(symbols, expr->expr.assign->expr);
    } else if (expr->type == EXPR_TERNARY) {
        typecheck_expr(symbols, expr->expr.ternary->condition);
        typecheck_expr(symbols, expr->expr.ternary->condition_true);
        typecheck_expr(symbols, expr->expr.ternary->condition_false);
    }
}

void typecheck_optional_expr(symboltable* symbols, expr_node* expr) {
    if (expr == NULL)
        return;
    typecheck_expr(symbols, expr);
}

void typecheck_variable_declaration(symboltable* symbols, variable_declaration_node* declare) {
    symboltable_add(symbols, declare->identifier, TYPE_INT, NO_PARAMS, 1);
    if (declare->init != NULL) {
        typecheck_expr(symbols, declare->init);
    }
}

void typecheck_block(symboltable* symbols, block_node* block) {
    for (int i = 0; i < block->items->len; i++) {
        block_item_node* item = (block_item_node*)list_get(block->items, i);
        if (item->type == BLOCK_DECLARE) {
            typecheck_declaration(symbols, item->item.declare);
        } else {
            typecheck_statement(symbols, item->item.stmt);
        }
    }
}

void typecheck_function_declaration(symboltable* symbols, function_declaration_node* declare) {
    int n_params = declare->params->len;
    int has_body = (declare->body != NULL) ? 1 : 0;
    int already_defined = 0;

    symboltable_node* node = symboltable_get(symbols, declare->name);
    if (node != NULL) {
        if (node->type_spec->type != TYPE_FUNC || node->type_spec->param_count != n_params) {
            char err_message[256];
            sprintf(err_message, "Incompatible function declaration of %s", declare->name);
            type_error(symbols, err_message);
        }
        already_defined = node->defined;
        if (already_defined && has_body) {
            char err_message[256];
            sprintf(err_message, "Function %s defined more than once", declare->name);
            type_error(symbols, err_message);
        }
    }

    int is_defined = already_defined || has_body;
    symboltable_add(symbols, declare->name, TYPE_FUNC, n_params, is_defined);

    if (has_body) {
        for (int i = 0; i < declare->params->len; i++) {
            param_node* param = (param_node*)list_get(declare->params, i);
            symboltable_add(symbols, param->identifier, TYPE_INT, NO_PARAMS, 1);
        }
        typecheck_block(symbols, declare->body);
    }
}

void typecheck_declaration(symboltable* symbols, declaration_node* declare) {
    if (declare->type == DECLARE_VARIABLE) {
        typecheck_variable_declaration(symbols, declare->declaration.variable);
    } else {
        typecheck_function_declaration(symbols, declare->declaration.function);
    }
}

void typecheck_statement(symboltable* symbols, statement_node* stmt) {
    if (stmt->type == STMT_RET) {
        typecheck_expr(symbols, stmt->stmt.ret->expr);
    } else if (stmt->type == STMT_EXPR) {
        typecheck_expr(symbols, stmt->stmt.expr);
    } else if (stmt->type == STMT_IF) {
        typecheck_expr(symbols, stmt->stmt.if_stmt->condition);
        typecheck_statement(symbols, stmt->stmt.if_stmt->then_stmt);
        if (stmt->stmt.if_stmt->else_stmt != NULL) {
            typecheck_statement(symbols, stmt->stmt.if_stmt->else_stmt);
        }
    } else if (stmt->type == STMT_COMPOUND) {
        typecheck_block(symbols, stmt->stmt.compound->block);
    } else if (stmt->type == STMT_WHILE) {
        typecheck_expr(symbols, stmt->stmt.while_node->condition);
        if (stmt->stmt.while_node->body != NULL) {
            typecheck_statement(symbols, stmt->stmt.while_node->body);
        }
    } else if (stmt->type == STMT_DO_WHILE) {
        if (stmt->stmt.do_while_node->body != NULL) {
            typecheck_statement(symbols, stmt->stmt.do_while_node->body);
        }
        typecheck_expr(symbols, stmt->stmt.do_while_node->condition);
    } else if (stmt->type == STMT_FOR) {
        if (stmt->stmt.for_node->init->type == INIT_DECL) {
            typecheck_variable_declaration(symbols, stmt->stmt.for_node->init->for_init.init_declare);
        } else {
            typecheck_expr(symbols, stmt->stmt.for_node->init->for_init.init_expr);
        }
        typecheck_optional_expr(symbols, stmt->stmt.for_node->condition);
        typecheck_optional_expr(symbols, stmt->stmt.for_node->final_expr);
        typecheck_statement(symbols, stmt->stmt.for_node->body);
    }
}

void typecheck_program(symboltable* symbols, program_node* program) {
    for (int i = 0; i < program->functions->len; i++) {
        function_declaration_node* func = (function_declaration_node*)list_get(program->functions, i);
        typecheck_function_declaration(symbols, func);
    }
}