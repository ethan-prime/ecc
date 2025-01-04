#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"
#include "../ast/ast.h"
#include "precedence.h"

#pragma once

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue* tq, token_id id);

// parses and returns corresponding ast nodes
constant_node* parse_constant(token_queue* tq);

unary_node* parse_unary_expr(token_queue* tq);
expr_node* parse_factor(token_queue* tq);
expr_node* parse_expr(token_queue* tq, int min_precedence);

expr_node* expr_node_copy(expr_node* expr); // used in parsing

return_node* parse_return(token_queue* tq);

variable_declaration_node* parse_variable_declaration(token_queue* tq);
function_declaration_node* parse_function_declaration(token_queue* tq);
declaration_node* parse_declaration(token_queue* tq);

statement_node* parse_statement(token_queue* tq);
compound_node* parse_compound_statement(token_queue* tq);

block_node* parse_block(token_queue* tq);

program_node* parse_program(token_queue* tq);

// prints a parser error and quits the program
void parser_error(char* expected, token* received);