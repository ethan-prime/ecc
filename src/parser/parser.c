#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"
#include "parser.h"

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue* tq, token_id id) {
    return (token_queue_cur(tq)->id == id) ? 1 : 0;
}

