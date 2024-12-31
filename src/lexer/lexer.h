#include "tokens.h"
#include "../utils/tokenqueue.h"

#pragma once

typedef struct lexer {
    char* input_file;
    size_t file_size;
    char* buf;
    unsigned int pos;
    char cur_char;
    int line_number;
} lexer_t;

// initializes a lexer struct and returns it.
lexer_t* lexer_init(char* source_file);

// reads the input file contents into the lexer buffer. returns 0 if successful.
int lexer_read_file(lexer_t* lexer);

// reads current char in buffer and moves to next char.
char lexer_read_char(lexer_t* lexer);

char lexer_peek_char(lexer_t* lexer);

token* lexer_next_token(lexer_t* lexer);

// checks if a str is a keyword. if it is, returns 1 and sets *id to the corresponding token_id.
// else, returns 0. this is an identifier or string literal.
int is_keyword(char* str, token_id* id);

// prints token information
void print_token(token t);

// tokenizes the contents of *buf.
token_queue* lexer_tokenize(lexer_t* lexer);