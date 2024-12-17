#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"

#pragma once

// returns true if head matches token_id, false otherwise
bool expect(token_queue* tq, token_id);