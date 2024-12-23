#include "ast.h"
#include <stdio.h>

void print_tabs(int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("  ");
    }
}

void print_constant(constant_node *constant) {
    printf("Constant(%d)\n", constant->val);
}

void print_binop(binary_op op) {
    if (op == ADD) {
        printf("+");
    } else if (op == SUBTRACT) {
        printf("-");
    } else if (op == MULTIPLY) {
        printf("*");
    } else if (op == DIVIDE) {
        printf("/");
    } else if (op == REMAINDER) {
        printf("%%");
    } else if (op == BITWISE_AND) {
        printf("&");
    } else if (op == BITWISE_OR) {
        printf("|");
    } else if (op == BITWISE_XOR) {
        printf("^");
    } else if (op == BITWISE_LEFT_SHIFT) {
        printf("<<");
    } else if (op == BITWISE_RIGHT_SHIFT) {
        printf(">>");
    }
}

void print_expr(expr_node *expr, int depth) {
    print_tabs(depth);
    if (expr->type == EXPR_CONSTANT) {
        print_constant(expr->expr.constant);
    } else if (expr->type == EXPR_UNARY) {
        printf("Unary(");
        if (expr->expr.unary_expr->op == COMPLEMENT) {
            printf("~,\n");
        } else if (expr->expr.unary_expr->op == NEGATE) {
            printf("Negate,\n");
        }
        print_expr(expr->expr.unary_expr->expr, depth+1);
        print_tabs(depth);
        printf(")\n");
    } else if (expr->type == EXPR_BINARY) {
        printf("Binary(");
        print_binop(expr->expr.binary_expr->op);
        printf(",\n");
        print_expr(expr->expr.binary_expr->lhs, depth+1);
        print_expr(expr->expr.binary_expr->rhs, depth+1);
        print_tabs(depth);
        printf(")\n");
    }
}

void print_return(return_node *ret, int depth) {
    printf("Return(\n");
    print_expr(ret->expr, depth+1);
    print_tabs(depth);
    printf(")\n");
}

void print_statement(statement_node* stmt, int depth) {
    print_return(stmt->ret, depth);
}

void print_function(function_node* func, int depth) {
    print_tabs(depth);
    printf("Function(\n");
    print_tabs(depth+1);
    printf("name=\"%s\",\n", func->identifier);
    print_tabs(depth+1);
    printf("body=");
    print_statement(func->body, depth+1);
    print_tabs(depth);
    printf(")\n");
}

// pretty-prints the ast of the program.
void print_ast(program_node *program) {
    printf("Program(\n");
    print_function(program->function, 1);
    printf(")\n");
}