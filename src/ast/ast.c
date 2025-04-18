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

void print_function_call(function_call_node* function_call, int depth) {
    printf("FunctionCall(\n");
    print_tabs(depth+1);
    printf("name=%s\n", function_call->identifier);
    print_tabs(depth+1);
    if (function_call->args->len == 0) {
        printf("args=None\n");
    } else {
        printf("args=\n");
        for(int i = 0; i < function_call->args->len; i++) {
            expr_node* arg = (expr_node*)list_get(function_call->args, i);
            print_expr(arg, depth+1);
        }
    }
    print_tabs(depth);
    printf(")\n");
}

void print_expr(expr_node *expr, int depth) {
    print_tabs(depth);
    if (expr == NULL) {
        printf("NullExpr()\n");
        return;
    }
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
    } else if (expr->type == EXPR_FUNCTION_CALL) {
        print_function_call(expr->expr.function_call, depth);
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

void print_block(block_node* block, int depth) {
    for (int i = 0; i < block->items->len; i++) {
        block_item_node* block_item = (block_item_node*)list_get(block->items, i);
        print_block_item(block_item, depth);
    }
}

void print_variable_declare(variable_declaration_node* declare, int depth) {
    print_tabs(depth);
    printf("VariableDeclare(\n");
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

void print_break(break_node* break_node, int depth) {
    print_tabs(depth);
    printf("Break(%s)\n",break_node->loop_label);
}

void print_continue(continue_node* continue_node, int depth) {
    print_tabs(depth);
    printf("Continue[%s]\n",continue_node->loop_label);
}

void print_while(while_node* while_node, int depth) {
    print_tabs(depth);
    printf("While[%s](\n", while_node->loop_label);
    print_tabs(depth+1);
    printf("condition=\n");
    print_expr(while_node->condition, depth+1);
    print_tabs(depth+1);
    printf("body=\n");
    print_statement(while_node->body, depth+1);
    print_tabs(depth);
    printf(")\n");
}

void print_do_while(do_while_node* do_while, int depth) {
    print_tabs(depth);
    printf("DoWhile[%s](\n", do_while->loop_label);
    print_tabs(depth+1);
    printf("body=\n");
    print_statement(do_while->body, depth+1);
    print_tabs(depth+1);
    printf("condition=\n");
    print_expr(do_while->condition, depth+1);
    print_tabs(depth);
    printf(")\n");
}

void print_for_init(for_init_node* for_init, int depth) {
    if (for_init->type == INIT_DECL) {
        print_variable_declare(for_init->for_init.init_declare, depth);
    } else {
        print_expr(for_init->for_init.init_expr, depth);
    }
}

void print_for(for_node* for_node, int depth) {
    print_tabs(depth);
    printf("For[%s](\n", for_node->loop_label);
    print_tabs(depth+1);
    printf("init=\n");
    print_for_init(for_node->init, depth+1);
    print_tabs(depth+1);
    printf("condition=\n");
    print_expr(for_node->condition, depth+1);
    print_tabs(depth+1);
    printf("final=\n");
    print_expr(for_node->final_expr, depth+1);
    print_tabs(depth+1);
    printf("body=\n");
    print_statement(for_node->body, depth+1);
    print_tabs(depth);
    printf(")\n");
}

void print_statement(statement_node* stmt, int depth) {
    switch(stmt->type) {
        case STMT_RET:
            print_return(stmt->stmt.ret, depth);
            break;
        case STMT_NULL:
            print_tabs(depth);
            printf("NullStatement()\n");
            break;
        case STMT_EXPR:
            print_expr(stmt->stmt.expr, depth);
            break;
        case STMT_IF:
            print_if(stmt->stmt.if_stmt, depth);
            break;
        case STMT_COMPOUND:
            print_tabs(depth);
            printf("{\n");
            print_block(stmt->stmt.compound->block, depth+1);
            print_tabs(depth);
            printf("}\n");
            break;
        case STMT_BREAK:
            print_break(stmt->stmt.break_node, depth);
            break;
        case STMT_CONTINUE:
            print_continue(stmt->stmt.continue_node, depth);
            break;
        case STMT_WHILE:
            print_while(stmt->stmt.while_node, depth);
            break;
        case STMT_DO_WHILE:
            print_do_while(stmt->stmt.do_while_node, depth);
            break;
        case STMT_FOR:
            print_for(stmt->stmt.for_node, depth);
            break;
    }
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

void print_function_declare(function_declaration_node* func, int depth) {
    print_tabs(depth);
    printf("FunctionDeclare(\n");
    print_tabs(depth+1);
    printf("name=\"%s\",\n", func->name);
    print_tabs(depth+1);
    if (func->params->len == 0) {
        printf("params=None\n");
    } else {
        printf("params=\n");
        print_tabs(depth+1);
        for (int i = 0; i < func->params->len; i++) {
            param_node* param = (param_node*)list_get(func->params, i);
            printf("%s, ", param->identifier);
        }
        printf("\n");
    }
    if (func->body != NULL) {
        print_tabs(depth+1);
        printf("body=\n");
        print_block(func->body, depth + 1);
    }
    print_tabs(depth);
    printf(")\n");
}

void print_declare(declaration_node* decl, int depth) {
    if (decl->type == DECLARE_FUNCTION) {
        print_function_declare(decl->declaration.function, depth);
    } else {
        print_variable_declare(decl->declaration.variable, depth);
    }
}

// pretty-prints the ast of the program.
void print_ast(program_node *program) {
    printf("Program(\n");
    for (int i = 0; i < program->functions->len; i++) {
        function_declaration_node* function = (function_declaration_node*)list_get(program->functions, i);
        print_function_declare(function, 1);
    }
    printf(")\n");
}