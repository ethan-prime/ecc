#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"
#include "../ast/ast.h"

#pragma once

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue* tq, token_id id);

// parses and returns corresponding ast nodes
constant_node* parse_constant(token_queue* tq);

expr_node* parse_expr(token_queue* tq);

return_node* parse_return(token_queue* tq);

statement_node* parse_statement(token_queue* tq);

function_node* parse_function(token_queue* tq);

program_node* parse_program(token_queue* tq);

// prints a parser error and quits the program
void parser_error(char* expected, token* received);