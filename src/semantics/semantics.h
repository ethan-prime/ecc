// implementation of semantic checking.

#include "../parser/parser.h"
#include "../utils/variablemap.h"

#pragma once

// used for throwing compiler errors
void semantic_error_declared_twice(char* identifier, variablemap* vm);
void semantic_error_undefined(char* identifier, variablemap* vm);
void semantic_error(char* message);

// look for invalid accesses, replace user-defined variables with global, temp vars.
void resolve_expr(variablemap* vm, expr_node* expr);
void resolve_declaration(variablemap* vm, declaration_node* declare);
void resolve_statement(variablemap* vm, statement_node* statement);
void resolve_block_item(variablemap* vm, block_item_node* block_item);
void resolve_block(variablemap* vm, block_node* block);
void resolve_function(function_node* function);
void resolve_program(program_node* program);
#define semantic_pass1(p) (resolve_program(p)) // variable resolution pass

void label_statement(statement_node* statement, char* loop_label);
void label_block(block_node* block, char* loop_label);
void label_function(function_node* func);
void label_program(program_node* program);
#define semantic_pass2(p) (label_program(p)) // loop labeling pass