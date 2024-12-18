#include "../lexer/tokens.h"
#include "../lexer/lexer.h"
#include "tokenqueue.h"
#include <stdlib.h>

// init token queue
token_queue* token_queue_init() {
    return (token_queue *)malloc(sizeof(token_queue));
}

// creates a token node from a token
token_node* token_node_create(token* tok) {
    token_node* tn = (token_node *)malloc(sizeof(token_node));

    tn->tok = tok;
    tn->next = NULL;

    return tn;
}

// add token to end of token queue
void token_queue_enq(token_queue* tq, token* tok) {
    if (tq->head == NULL) {
        tq->head = token_node_create(tok);
        return;
    }

    token_node* curr = tq->head;
    while (curr->next != NULL) {
        curr = curr->next;
    }

    curr->next = token_node_create(tok);
}

// returns head of queue, removes from queue.
token* token_queue_deq(token_queue* tq) {
    if (tq->head == NULL) {
        return NULL;
    }

    token* tmp = tq->head->tok;
    tq->head = tq->head->next;

    return tmp;
}

// returns head of queue, doesnt remove.
token* token_queue_cur(token_queue* tq) {
    if (tq->head == NULL) {
        return NULL;
    }
    return tq->head->tok;
}

// returns two tokens into queue, doesnt remove.
token* token_queue_peek(token_queue* tq) {
    if (tq->head->next == NULL || tq->head == NULL) {
        return NULL;
    }

    return tq->head->next->tok;
}

// prints the token queue contents
void print_token_queue(token_queue* tq) {
    token_node* curr = tq->head;

    while (curr != NULL) {
        print_token(*curr->tok);
        curr = curr->next;
    }
}