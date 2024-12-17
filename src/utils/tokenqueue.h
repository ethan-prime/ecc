#include "../lexer/tokens.h"

#pragma once

typedef struct token_node {
    token* tok;
    struct token_node* next;
} token_node;

typedef struct {
    token_node* head;
} token_queue;

// inits a token queue, returns
token_queue* token_queue_init();

// creates a token node from a token
token_node* token_node_create(token* tok);

// used for interacting with tokenqueue datastructure
void token_queue_enq(token_queue* tq, token* tok);

// returns head of queue, removes from queue.
token* token_queue_deq(token_queue* tq);

// returns head of queue, doesnt remove.
token* token_queue_cur(token_queue* tq);

// returns two tokens into queue, doesnt remove.
token* token_queue_peek(token_queue* tq);

// prints the token queue contents
void print_token_queue(token_queue* tq);