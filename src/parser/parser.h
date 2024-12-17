#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"

#pragma once

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue* tq, token_id id);