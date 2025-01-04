// implementation of semantic checking.
#include "../config.h"
#if SEMANTICS_ENABLE
#include "../parser/parser.h"
#include "../utils/identifiermap.h"

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

#if 0
void label_statement(statement_node *statement, char *loop_label);
void label_block(block_node *block, char *loop_label);
void label_function(function_node *func);
void label_program(program_node *program);
#define semantic_pass2(p) (label_program(p)) // loop labeling pass
#endif
#endif