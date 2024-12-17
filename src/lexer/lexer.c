#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "tokens.h"
#include "lexer.h"
#include <string.h>
#include "../utils/tokenqueue.h"

// initializes a lexer struct and returns it.
lexer_t* lexer_init(char* source_file) {
    lexer_t* lexer = (lexer_t *)malloc(sizeof(lexer_t));
    lexer->input_file = source_file;
    lexer->pos = 0;
    return lexer;
}

// reads the input file contents into the lexer buffer. returns 0 if successful.
int lexer_read_file(lexer_t* lexer) {
    FILE *file = fopen(lexer->input_file, "rb"); // open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate a buffer to store the file contents
    lexer->buf = (char *)malloc(file_size + 1); // +1 for the null terminator
    if (lexer->buf == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // read the file contents into the buffer
    size_t bytes_read = fread(lexer->buf, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        fclose(file);
        return 1;
    }

    lexer->file_size = file_size;
    lexer_read_char(lexer); // read first character
    return 0; //success
}

// reads current char in buffer and moves to next char.
char lexer_read_char(lexer_t* lexer) {
    char c = lexer->buf[lexer->pos];
    lexer->cur_char = c;
    lexer->pos++;
    return c;
}

char lexer_peek_char(lexer_t* lexer) {
    return lexer->buf[lexer->pos];
}

// checks if a str is a keyword. if it is, returns 1 and sets *id to the corresponding token_id.
// else, returns 0. this is an identifier or string literal.
int is_keyword(char* str, token_id* id) {
    if (strcmp(str, "int") == 0) {
        *id = KEYW_INT;
        return 1;
    } else if (strcmp(str, "void") == 0) {
        *id = KEYW_VOID;
        return 1;
    } else if (strcmp(str, "return") == 0) {
        *id = KEYW_RETURN;
        return 1;
    }
    return 0;
}

// gets the next token in the buffer and retunrs it.
token* lexer_next_token(lexer_t* lexer) {
    token* t = (token*)malloc(sizeof(token));

    if (lexer->pos >= lexer->file_size) {
        return NULL;
    }

    // skip whitespace
    while (isspace(lexer->cur_char) && lexer->pos < lexer->file_size) {
        lexer_read_char(lexer);
    }

    char buf[1024];
    unsigned int index = 0;

    if (isdigit(lexer->cur_char)) {
        // read number into buffer
        do {
            buf[index] = lexer->cur_char;
            index++;
            lexer_read_char(lexer);
        } while (isdigit(lexer->cur_char) && lexer->pos < lexer->file_size);
        buf[index] = '\0';

        t->id = CONSTANT;
        t->val.i = atoi(buf);

    } else if (isalpha(lexer->cur_char)) {
        // read identifier/keyword into buffer
        do {
            buf[index] = lexer->cur_char;
            index++;
            lexer_read_char(lexer);
        } while (isalnum(lexer->cur_char) && lexer->pos < lexer->file_size);
        buf[index] = '\0';

        t->val.s = malloc((strlen(buf)+1)*sizeof(char));
        strcpy(t->val.s, buf);

        if (!is_keyword(t->val.s, &t->id)) {
            t->id = IDENTIFIER;
        }
    
    } else if (lexer->cur_char == '(') {
        t->id = OPEN_PAREN;
        lexer_read_char(lexer);
    } else if (lexer->cur_char == ')') {
        t->id = CLOSE_PAREN;
        lexer_read_char(lexer);
    } else if (lexer->cur_char == '{') {
        t->id = OPEN_BRACE;
        lexer_read_char(lexer);
    } else if (lexer->cur_char == '}') {
        t->id = CLOSE_BRACE;
        lexer_read_char(lexer);
    } else if (lexer->cur_char == ';') {
        t->id = SEMICOLON;
        lexer_read_char(lexer);
    } else {
        printf("UNKNOWN TOKEN ENCOUNTERED\n");
        exit(1);
    }

    return t;
}

// prints token information
void print_token(token t) {
    switch (t.id) {
        case KEYW_INT:
            printf("int"); break;
        case IDENTIFIER:
            printf("identifier (%s)", t.val.s); break;
        case OPEN_PAREN:
            printf("("); break;
        case CLOSE_PAREN:
            printf(")"); break;
        case KEYW_VOID:
            printf("void"); break;
        case OPEN_BRACE:
            printf("{"); break;
        case CLOSE_BRACE:
            printf("}"); break;
        case KEYW_RETURN:
            printf("return"); break;
        case CONSTANT:
            printf("constant (%d)", t.val.i); break;
        case SEMICOLON:
            printf(";"); break;
        default:
            printf("UNKNOWN TOKEN\n");
            exit(1);
            break;
    }
    printf("\n");
};

// tokenizes the contents of *buf.
// returns token queue of tokenized input.
token_queue* lexer_tokenize(lexer_t* lexer) {
    token_queue* tq = token_queue_init();

    token* t = lexer_next_token(lexer);
    while (t != NULL) {
        // add token to token queue
        token_queue_enq(tq, t);
        t = lexer_next_token(lexer);
    }

    //print_token_queue(tq);

    return tq;
}
