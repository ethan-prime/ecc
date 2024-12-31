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
    } else if (op == LOGICAL_AND) {
        printf("&&");
    } else if (op == LOGICAL_OR) {
        printf("||");
    } else if (op == LOGICAL_EQUAL) {
        printf("==");
    } else if (op == LOGICAL_NOT_EQUAL) {
        printf("!=");
    } else if (op == LOGICAL_LT) {
        printf("<");
    } else if (op == LOGICAL_LTE) {
        printf("<=");
    } else if (op == LOGICAL_GT) {
        printf(">");
    } else if (op == LOGICAL_GTE) {
        printf(">=");
    } else if (op == PREFIX_INC) {
        printf("++prefix");
    } else if (op == POSTFIX_INC) {
        printf("postfix++");
    } else if (op == PREFIX_DEC) {
        printf("--prefix");
    } else if (op == POSTFIX_DEC) {
        printf("postfix--");
    }
}

void print_ternary(ternary_node* ternary, int depth) {
    printf("Ternary(\n");
    print_tabs(depth+1);
    printf("condition=\n");
    print_expr(ternary->condition, depth+1);
    print_tabs(depth+1);
    printf("condition_true=\n");
    print_expr(ternary->condition_true, depth+1);
    print_tabs(depth+1);
    printf("condition_false=\n");
    print_expr(ternary->condition_false, depth+1);
    print_tabs(depth);
    printf(")\n");
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
            printf("-,\n");
        } else if (expr->expr.unary_expr->op == NOT) {
            printf("!,\n");
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
    } else if (expr->type == EXPR_ASSIGN) {
        printf("Assign(\n");
        print_expr(expr->expr.assign->lvalue, depth+1);
        print_expr(expr->expr.assign->expr, depth+1);
        print_tabs(depth);
        printf(")\n");
    } else if (expr->type == EXPR_VARIABLE) {
        printf("Var(%s)\n", expr->expr.variable->identifier);
    } else if (expr->type == EXPR_TERNARY) {
        print_ternary(expr->expr.ternary, depth);
    }
}

void print_return(return_node *ret, int depth) {
    print_tabs(depth);
    printf("Return(\n");
    print_expr(ret->expr, depth+1);
    print_tabs(depth);
    printf(")\n");
}

void print_if(if_node* if_node, int depth) {
    print_tabs(depth);
    printf("If(\n");
    print_tabs(depth+1);
    printf("condition=\n");
    print_expr(if_node->condition, depth+1);
    print_tabs(depth+1);
    printf("then=\n");
    print_statement(if_node->then_stmt, depth+1);
    if (if_node->else_stmt != NULL) {
        // we have an else
        print_tabs(depth+1);
        printf("else=\n");
        print_statement(if_node->else_stmt, depth+1);
    }
    print_tabs(depth);
    printf(")\n");
}

void print_statement(statement_node* stmt, int depth) {
    switch(stmt->type) {
        case STMT_RET:
            print_return(stmt->stmt.ret, depth);
            break;
        case STMT_NULL:
            printf("Null()\n");
            break;
        case STMT_EXPR:
            print_expr(stmt->stmt.expr, depth);
            break;
        case STMT_IF:
            print_if(stmt->stmt.if_stmt, depth);
            break;
    }
}

void print_declare(declaration_node* declare, int depth) {
    print_tabs(depth);
    printf("Declare(\n");
    print_tabs(depth+1);
    printf("Var(%s)", declare->identifier);
    if (declare->init != NULL) {
        printf("\n");
        print_expr(declare->init, depth+1);
    } else {
        printf("\n");
    }
    print_tabs(depth);
    printf(")\n");
}

void print_block_item(block_item_node* block_item, int depth) {
    switch (block_item->type) {
        case BLOCK_DECLARE:
            print_declare(block_item->item.declare, depth);
            break;
        case BLOCK_STMT:
            print_statement(block_item->item.stmt, depth);
            break;
    }
}

void print_function(function_node* func, int depth) {
    print_tabs(depth);
    printf("Function(\n");
    print_tabs(depth+1);
    printf("name=\"%s\",\n", func->identifier);
    print_tabs(depth+1);
    printf("body=\n");
    for (int i = 0; i < func->body->len; i++) {
        block_item_node* block_item = (block_item_node*)list_get(func->body, i);
        print_block_item(block_item, depth+1);
    }
    print_tabs(depth);
    printf(")\n");
}

// pretty-prints the ast of the program.
void print_ast(program_node *program) {
    printf("Program(\n");
    print_function(program->function, 1);
    printf(")\n");
}