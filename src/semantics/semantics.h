// implementation of semantic checking.

#include "../parser/parser.h"
#include "../utils/variablemap.h"

#pragma once

// used for throwing compiler errors
void semantic_error_declared_twice(char* identifier);
void semantic_error_undefined(char* identifier);
void semantic_error(char* message);

// look for invalid accesses, replace user-defined variables with global, temp vars.
void resolve_expr(variablemap* vm, expr_node* expr);
void resolve_declaration(variablemap* vm, declaration_node* declare);
void resolve_statement(variablemap* vm, statement_node* statement);
void resolve_block_item(variablemap* vm, block_item_node* block_item);
void resolve_function(function_node* function);
void resolve_program(program_node* program);
#define semantic_pass1(p) (resolve_program(p)) // variable resolution pass