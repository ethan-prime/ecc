#include <stdio.h>

#pragma once

typedef enum {
    KEYW_INT,
    IDENTIFIER,
    OPEN_PAREN,
    CLOSE_PAREN,
    KEYW_VOID,
    OPEN_BRACE,
    KEYW_RETURN,
    CONSTANT,
    SEMICOLON,
    CLOSE_BRACE,
    TILDE,
    HYPHEN,
    HYPHEN_HYPHEN,
    PLUS,
    ASTERISK,
    FORWARD_SLASH,
    PERCENT,
} token_id;

typedef union {
    int i;
    double d;
    char* s;
} token_value;

typedef struct {
    token_id id;
    token_value val;
} token;