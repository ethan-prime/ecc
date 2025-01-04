#include "../config.h"
#if SEMANTICS_ENABLE
#include "semantics.h"
#include <stdlib.h>
#include "../ir/ir.h"
#include <string.h>

int USER_DEFINED_COUNTER = 0;
int PARAM_COUNTER = 0;

void semantic_error(char *message)
{
    printf("***semantic error:***\n");
    printf("%s\n", message);
    exit(1);
}

void semantic_error_declared_twice(char *identifier, identifiermap *im)
{
    printf("***semantic error:***\n");
    printf("variable %s redeclared\n", identifier);
    printf("variable map at exit:\n");
    identifiermap_print(im);
    exit(1);
}

void semantic_error_undefined(char *identifier, identifiermap *im)
{
    printf("***semantic error:***\n");
    printf("variable %s undefined\n", identifier);
    printf("variable map at exit:\n");
    identifiermap_print(im);
    exit(1);
}

void resolve_expr(identifiermap *im, expr_node *expr)
{
    if (expr->type == EXPR_ASSIGN)
    {
        assign_node *assign = expr->expr.assign;
        if (assign->lvalue->type != EXPR_VARIABLE)
        {
            // lvalue isnt a variable, this is an error...
            semantic_error("invalid lvalue for assignment!");
        }

        resolve_expr(im, assign->lvalue);
        resolve_expr(im, assign->expr);
    }
    else if (expr->type == EXPR_VARIABLE)
    {
        identifiermap_node *res = identifiermap_get(im, expr->expr.variable->identifier);
        if (res == NULL)
        {
            semantic_error_undefined(expr->expr.variable->identifier, im);
        }
        // set it to new temp variable
        expr->expr.variable->identifier = res->value;

        // recursively resolve other expressions
    }
    else if (expr->type == EXPR_UNARY)
    {
        resolve_expr(im, expr->expr.unary_expr->expr);
    }
    else if (expr->type == EXPR_BINARY)
    {
        resolve_expr(im, expr->expr.binary_expr->lhs);
        resolve_expr(im, expr->expr.binary_expr->rhs);
    }
    else if (expr->type == EXPR_TERNARY)
    {
        resolve_expr(im, expr->expr.ternary->condition);
        resolve_expr(im, expr->expr.ternary->condition_true);
        resolve_expr(im, expr->expr.ternary->condition_false);
    } else if (expr->type == EXPR_FUNCTION_CALL) {
        // make sure function is defined
        identifiermap_node *res = identifiermap_get(im, expr->expr.function_call->identifier);
        if (res == NULL)
        {
            semantic_error_undefined(expr->expr.function_call->identifier, im);
        }
        // resolve all arguments of function call
        for (int i = 0; i < expr->expr.function_call->args->len; i++) {
            resolve_expr(im, (expr_node*)list_get(expr->expr.function_call->args, i));
        }
    }
}

void resolve_variable_declaration(identifiermap *im, variable_declaration_node *declare)
{
    if (identifiermap_get(im, declare->identifier) && from_current_scope(im, declare->identifier))
    {
        semantic_error_declared_twice(declare->identifier, im);
    }

    char *prefix = (char *)malloc(sizeof(char) * (strlen(declare->identifier) + 2)); // . and \0
    sprintf(prefix, "%s.", declare->identifier);
    char *temp_name = ir_make_n_ident(prefix, USER_DEFINED_COUNTER);
    USER_DEFINED_COUNTER++;

    identifiermap_add(im, declare->identifier, temp_name, NO_LINKAGE); // update map to reflect new temp var mapping.
    declare->identifier = temp_name;                       // set identifier to new name.
    if (declare->init != NULL)
    {
        resolve_expr(im, declare->init);
    }
}

void resolve_param(identifiermap* im, param_node* param) {
    if (identifiermap_get(im, param->identifier) && from_current_scope(im, param->identifier))
    {
        semantic_error_declared_twice(param->identifier, im);
    }

    char* temp_name = ir_make_n_ident("param.", PARAM_COUNTER++);
    identifiermap_add(im, param->identifier, temp_name, NO_LINKAGE);
    param->identifier = temp_name;
}

void resolve_function_declaration(identifiermap* im, function_declaration_node* function_declare) {
    identifiermap_node* entry = identifiermap_get(im, function_declare->name);
    if (entry != NULL) {
        if (entry->from_current_scope && !entry->has_linkage) {
            semantic_error_declared_twice(function_declare->name, im);
        }
    }

    identifiermap_add(im, function_declare->name, function_declare->name, HAS_LINKAGE);

    identifiermap* inner = identifiermap_copy(im);

    for (int i = 0; i < function_declare->params->len; i++) {
        param_node* param = (param_node*)list_get(function_declare->params, i);
        resolve_param(inner, param);
    }

    if (function_declare->body != NULL) {
        resolve_block(inner, function_declare->body);
    }
}

void resolve_declaration(identifiermap* im, declaration_node* declare) {
    if (declare->type == DECLARE_VARIABLE) {
        resolve_variable_declaration(im, declare->declaration.variable);
    } else if (declare->type == DECLARE_FUNCTION) {
        // if we reached this point, we are in a local scope? probably??
        if (declare->declaration.function->body != NULL) {
            semantic_error("cannot define a function within a local scope.");
        }
        resolve_function_declaration(im, declare->declaration.function);
    }
}

void resolve_if(identifiermap *im, if_node *if_stmt)
{
    resolve_expr(im, if_stmt->condition);
    resolve_statement(im, if_stmt->then_stmt);
    if (if_stmt->else_stmt != NULL)
    {
        resolve_statement(im, if_stmt->else_stmt);
    }
}

void resolve_while(identifiermap *im, while_node *while_stmt)
{
    resolve_expr(im, while_stmt->condition);
    resolve_statement(im, while_stmt->body);
}

void resolve_do_while(identifiermap *im, do_while_node *do_while_stmt)
{
    resolve_statement(im, do_while_stmt->body);
    resolve_expr(im, do_while_stmt->condition);
}

void resolve_optional_expr(identifiermap *im, expr_node *expr)
{
    if (expr == NULL)
    {
        return;
    }
    resolve_expr(im, expr);
}

void resolve_for_init(identifiermap *im, for_init_node *for_init)
{
    if (for_init->type == INIT_DECL)
    {
        resolve_variable_declaration(im, for_init->for_init.init_declare);
    }
    else
    {
        resolve_optional_expr(im, for_init->for_init.init_expr);
    }
}

void resolve_for(identifiermap *im, for_node *for_stmt)
{
    identifiermap *child_im = identifiermap_copy(im);
    resolve_for_init(child_im, for_stmt->init);
    resolve_optional_expr(child_im, for_stmt->condition);
    resolve_optional_expr(child_im, for_stmt->final_expr);
    resolve_statement(child_im, for_stmt->body);
}

void resolve_statement(identifiermap *im, statement_node *statement)
{
    if (statement->type == STMT_RET)
    {
        resolve_expr(im, statement->stmt.ret->expr);
    }
    else if (statement->type == STMT_EXPR)
    {
        resolve_expr(im, statement->stmt.expr);
    }
    else if (statement->type == STMT_IF)
    {
        resolve_if(im, statement->stmt.if_stmt);
    }
    else if (statement->type == STMT_COMPOUND)
    {
        identifiermap *child_im = identifiermap_copy(im);
        resolve_block(child_im, statement->stmt.compound->block);
    }
    else if (statement->type == STMT_WHILE)
    {
        resolve_while(im, statement->stmt.while_node);
    }
    else if (statement->type == STMT_DO_WHILE)
    {
        resolve_do_while(im, statement->stmt.do_while_node);
    }
    else if (statement->type == STMT_FOR)
    {
        resolve_for(im, statement->stmt.for_node);
    }
    // null, break, and continue statements need no resolution
}

void resolve_block_item(identifiermap *im, block_item_node *block_item)
{
    if (block_item->type == BLOCK_DECLARE)
    {
        resolve_declaration(im, block_item->item.declare);
    }
    else if (block_item->type == BLOCK_STMT)
    {
        resolve_statement(im, block_item->item.stmt);
    }
}

void resolve_block(identifiermap *im, block_node *block)
{
    block_item_node *block_item;
    for (int i = 0; i < block->items->len; i++)
    {
        block_item = (block_item_node *)list_get(block->items, i);
        resolve_block_item(im, block_item);
    }
}

void resolve_program(program_node *program)
{
    identifiermap* im = identifiermap_init();
    for (int i = 0; i < program->functions->len; i++) {
        function_declaration_node* func = (function_declaration_node*)list_get(program->functions, i);
        resolve_function_declaration(im, func);
    }
}

#endif