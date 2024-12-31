#include "semantics.h"
#include <stdlib.h>
#include "../ir/ir.h"
#include <string.h>

int USER_DEFINED_COUNTER = 0;

void semantic_error(char* message) {
    printf("***semantic error:***\n");
    printf("%s\n", message);
    exit(1);
}

void semantic_error_declared_twice(char* identifier, variablemap* vm) {
    printf("***semantic error:***\n");
    printf("variable %s redeclared\n", identifier);
    printf("variable map at exit:\n");
    variablemap_print(vm);
    exit(1);
}

void semantic_error_undefined(char* identifier, variablemap* vm) {
    printf("***semantic error:***\n");
    printf("variable %s undefined\n", identifier);
    printf("variable map at exit:\n");
    variablemap_print(vm);
    exit(1);
}

void resolve_expr(variablemap* vm, expr_node* expr) {
    if (expr->type == EXPR_ASSIGN) {
        assign_node* assign = expr->expr.assign;
        if (assign->lvalue->type != EXPR_VARIABLE) {
            // lvalue isnt a variable, this is an error...
            semantic_error("invalid lvalue for assignment!");
        }

        resolve_expr(vm, assign->lvalue);
        resolve_expr(vm, assign->expr);
    } else if (expr->type == EXPR_VARIABLE) {
        char* res = variablemap_get(vm, expr->expr.variable->identifier);
        if (res == NULL) {
            semantic_error_undefined(expr->expr.variable->identifier, vm);
        }
        // set it to new temp variable
        expr->expr.variable->identifier = res;
    
    // recursively resolve other expressions
    } else if (expr->type == EXPR_UNARY) {
        resolve_expr(vm, expr->expr.unary_expr->expr);
    }  else if (expr->type == EXPR_BINARY) {
        resolve_expr(vm, expr->expr.binary_expr->lhs);
        resolve_expr(vm, expr->expr.binary_expr->rhs);
    } else if (expr->type == EXPR_TERNARY) {
        resolve_expr(vm, expr->expr.ternary->condition);
        resolve_expr(vm, expr->expr.ternary->condition_true);
        resolve_expr(vm, expr->expr.ternary->condition_false);
    }
}

void resolve_declaration(variablemap* vm, declaration_node* declare) {
    if (variablemap_get(vm, declare->identifier) && from_current_block(vm, declare->identifier)) {
        semantic_error_declared_twice(declare->identifier, vm);
    }

    char* prefix = (char*)malloc(sizeof(char)*(strlen(declare->identifier)+2)); // . and \0
    sprintf(prefix, "%s.", declare->identifier);
    char* temp_name = ir_make_n_ident(prefix, USER_DEFINED_COUNTER);
    USER_DEFINED_COUNTER++;

    variablemap_add(vm, declare->identifier, temp_name); // update map to reflect new temp var mapping.
    declare->identifier = temp_name; // set identifier to new name.
    if (declare->init != NULL) {
        resolve_expr(vm, declare->init);
    }
}

void resolve_if(variablemap* vm, if_node* if_stmt) {
    resolve_expr(vm, if_stmt->condition);
    resolve_statement(vm, if_stmt->then_stmt);
    if (if_stmt->else_stmt != NULL) {
        resolve_statement(vm, if_stmt->else_stmt);
    }
}

void resolve_statement(variablemap* vm, statement_node* statement) {
    if (statement->type == STMT_RET) {
        resolve_expr(vm, statement->stmt.ret->expr);
    } else if (statement->type == STMT_EXPR) {
        resolve_expr(vm, statement->stmt.expr);
    } else if (statement->type == STMT_IF) {
        resolve_if(vm, statement->stmt.if_stmt);
    } else if (statement->type == STMT_COMPOUND) {
        variablemap* child_vm = variablemap_copy(vm);
        resolve_block(child_vm, statement->stmt.compound->block);
    }
    // null statement needs no resolution
}

void resolve_block_item(variablemap* vm, block_item_node* block_item) {
    if (block_item->type == BLOCK_DECLARE) {
        resolve_declaration(vm, block_item->item.declare);
    } else if (block_item->type == BLOCK_STMT) {
        resolve_statement(vm, block_item->item.stmt);
    }
}

void resolve_block(variablemap* vm, block_node* block) {
    block_item_node* block_item;
    for (int i = 0; i < block->items->len; i++) {
        block_item = (block_item_node*)list_get(block->items, i);
        resolve_block_item(vm, block_item);
    }
}

void resolve_function(function_node* function) {
    // each function gets its own variablemap, for now? i think
    variablemap* vm = variablemap_init();

    block_item_node* block_item;
    for (int i = 0; i < function->body->items->len; i++) {
        block_item = (block_item_node*)list_get(function->body->items, i);
        resolve_block_item(vm, block_item);
    }
}

void resolve_program(program_node* program) {
    resolve_function(program->function);
}