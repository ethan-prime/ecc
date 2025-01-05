// implementation of semantic checking.
#include "../config.h"
#if SEMANTICS_ENABLE
#include "../parser/parser.h"
#include "../utils/identifiermap.h"
#include "../utils/symboltable.h"

#pragma once

// used for throwing compiler errors
void semantic_error_declared_twice(char *identifier, identifiermap *im);
void semantic_error_undefined(char *identifier, identifiermap *im);
void semantic_error(char *message);

// look for invalid accesses, replace user-defined variables with global, temp vars.
void resolve_expr(identifiermap *im, expr_node *expr);
void resolve_declaration(identifiermap *im, declaration_node *declare);
void resolve_statement(identifiermap *im, statement_node *statement);
void resolve_block_item(identifiermap *im, block_item_node *block_item);
void resolve_block(identifiermap *im, block_node *block);
void resolve_program(program_node *program);
#define semantic_pass1(p) (resolve_program(p)) // variable resolution pass

// typechecking
void typecheck_expr(symboltable* symbols, expr_node* expr);
void typecheck_block(symboltable* symbols, block_node* block);
void typecheck_variable_declaration(symboltable* symbols, variable_declaration_node* declare);
void typecheck_function_declaration(symboltable* symbols, function_declaration_node* declare);
void typecheck_declaration(symboltable* symbols, declaration_node* declare);
void typecheck_block(symboltable* symbols, block_node* block);
void typecheck_statement(symboltable* symbols, statement_node* stmt);
void typecheck_program(symboltable* symbols, program_node* program);
#define semantic_pass2(p) (typecheck_program(p))

void label_statement(statement_node *statement, char *loop_label);
void label_block(block_node *block, char *loop_label);
void label_function_declaration(function_declaration_node *func);
void label_program(program_node *program);
#define semantic_pass3(p) (label_program(p)) // loop labeling pass
#endif