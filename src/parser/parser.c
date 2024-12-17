#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue* tq, token_id id) {
    return (token_queue_cur(tq)->id == id) ? 1 : 0;
}

// prints a parser error and quits the program
void parser_error(char* expected, token* received) {
    printf("*** ecc: syntax error ***\n");
    printf("expected: %s\n", expected);
    printf("received: ");
    print_token(*received);
    exit(1);
}

// parses and returns corresponding ast nodes
constant_node* parse_constant(token_queue* tq) {
    if (!expect(tq, CONSTANT)) {
        parser_error("a constant literal", token_queue_cur(tq));
    };
    
    
    constant_node* node = MALLOC(constant_node);
    node->val = token_queue_cur(tq)->val.i;
    token_queue_deq(tq);

    return node;
}

expr_node* parse_expr(token_queue* tq) {
    expr_node* node = MALLOC(expr_node);

    node->constant = parse_constant(tq);

    return node;
}

return_node* parse_return(token_queue* tq) {
    if (!expect(tq, KEYW_RETURN)) {
        parser_error("return", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    return_node* node = MALLOC(return_node);

    node->expr = parse_expr(tq);

    if (!expect(tq, SEMICOLON)) {
        parser_error(";", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    return node;
}

statement_node* parse_statement(token_queue* tq) {
    statement_node* node = MALLOC(statement_node);

    node->ret = parse_return(tq);

    return node;
}

function_node* parse_function(token_queue* tq) {
    if (!expect(tq, KEYW_INT)) {
        parser_error("int", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    function_node* node = MALLOC(function_node);

    if (!expect(tq, IDENTIFIER)) {
        parser_error("a function identifier", token_queue_cur(tq));
    }
    node->identifier = token_queue_cur(tq)->val.s;
    token_queue_deq(tq);

    if (!expect(tq, OPEN_PAREN)) {
        parser_error("(", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    // "void" is optional
    if (expect(tq, KEYW_VOID)) {
        token_queue_deq(tq);
    }
    
    if (!expect(tq, CLOSE_PAREN)) {
        parser_error(")", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    if (!expect(tq, OPEN_BRACE)) {
        parser_error("{", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    // parse a statement - later : multiple statements!
    node->body = parse_statement(tq);

    if (!expect(tq, CLOSE_BRACE)) {
        parser_error("}", token_queue_cur(tq));
    }
    token_queue_deq(tq);

    return node;
}

program_node* parse_program(token_queue* tq) {
    program_node* node = MALLOC(program_node);

    node->function = parse_function(tq);

    return node;
}