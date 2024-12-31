#include "semantics.h"
#include <stdlib.h>
#include "../ir/ir.h"
#include <string.h>

int LOOP_LABEL_IDX = 0;

char* make_loop_label() {
    char* res = ir_make_n_ident("_loop.", LOOP_LABEL_IDX);
    LOOP_LABEL_IDX++;
    return res;
}

void label_block(block_node* block, char* loop_label) {
    for (int i = 0; i < block->items->len; i++) {
        block_item_node* block_item = (block_item_node*)list_get(block->items, i);
        if (block_item->type == BLOCK_STMT) {
            label_statement(block_item->item.stmt, loop_label);
        }
    }
}

void label_statement(statement_node* statement, char* loop_label) {
    if (statement->type == STMT_BREAK) {
        if (loop_label == NULL) {
            semantic_error("break statement outside of loop");
        }
        statement->stmt.break_node->loop_label = loop_label;
    } else if (statement->type == STMT_CONTINUE) {
        if (loop_label == NULL) {
            semantic_error("continue statement outside of loop");
        }
        statement->stmt.continue_node->loop_label = loop_label;
    } else if (statement->type == STMT_WHILE) {
        char* new_label = make_loop_label();
        label_statement(statement->stmt.while_node->body, new_label);
        statement->stmt.while_node->loop_label = new_label;
    } else if (statement->type == STMT_DO_WHILE) {
        char* new_label = make_loop_label();
        label_statement(statement->stmt.do_while_node->body, new_label);
        statement->stmt.do_while_node->loop_label = new_label;
    } else if (statement->type == STMT_FOR) {
        char* new_label = make_loop_label();
        label_statement(statement->stmt.for_node->body, new_label);
        statement->stmt.for_node->loop_label = new_label;
    } else if (statement->type == STMT_IF) {
        label_statement(statement->stmt.if_stmt->then_stmt, loop_label);
        if (statement->stmt.if_stmt->else_stmt != NULL) {
            label_statement(statement->stmt.if_stmt->else_stmt, loop_label);
        }
    } else if (statement->type == STMT_COMPOUND) {
        label_block(statement->stmt.compound->block, loop_label);
    }
}

void label_function(function_node* func) {
    label_block(func->body, NULL);
}

void label_program(program_node* program) {
    label_function(program->function);
}