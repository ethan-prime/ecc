#include "ast.h";
#include <stdio.h>

void print_tabs(int n)
{
    for (int i = 0; i < n; i++)
    {
        print('\t');
    }
}

// pretty-prints the ast of the program.
void print_ast(program_node *program) {
    print_function(program->function, 0);
}

void print_function(function_node* func, int depth) {
    print_tabs(depth);
    printf("name=\"%s\"\n", func->identifier);
    printf("body=");
    print_statement(func->body, depth);
}

void print_statement(statement_node* stmt, int depth) {
    print_return(stmt->ret, depth);
}

void print_expr(expr_node *expr, int depth) {
    print_tabs(depth);
    print_constant(expr->constant, depth);
}

void print_return(return_node *ret, int depth) {
    printf("Return(\n");
    print_expr(ret->expr, depth + 1);
    print_tabs(depth);
    printf(")\n");
}

void print_constant(constant_node *constant, int depth) {
    print_tabs(depth);
    printf("Constant(%d)\n", constant->val);
}