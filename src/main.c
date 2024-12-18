#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer/lexer.h"
#include "lexer/tokens.h"
#include "parser/parser.h"
#include "assembly/asm_ast.h"
#include "codegen/codegen.h"

void panic(char* msg) {
    printf("%s\n", msg);
    exit(1);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        panic("not enough arguments. usage: ./ecc <path.c> [--lex, --parse, --codegen]");
    }

    char* source_file = argv[1];
    char* stage = argv[2];

    bool lex = false, parse = false, codegen = false;
    
    if (argc > 2) {
        if (strcmp(stage, "--lex") == 0) {
            lex = true;
        } else if (strcmp(stage, "--parse") == 0) {
            parse = true;
        } else if (strcmp(stage, "--codegen") == 0) {
            codegen = true;
        } else {
            printf("invalid option: %s\n", stage);
            panic("fatal error");
        }
    }

    lexer_t* lexer = lexer_init(source_file);
    lexer_read_file(lexer);

    /*
    printf("File contents:\n");
    printf("%s\n", lexer->buf);
    */

    token_queue* tq = lexer_tokenize(lexer);

    if (lex) {
        exit(0);
    }

    printf("Sucessfully lexed program...\n");

    program_node* program = parse_program(tq);

    printf("Parsed program:\n");
    print_ast(program);

    if (parse) {
        exit(0);
    }

    /*asm_program_node* program_asm = program_to_asm(program);

    printf("Succesfully codegened program...\n");

    if (codegen) {
        exit(0);
    }

    char* dest_file = (char*)malloc(sizeof((strlen(source_file)+1)*sizeof(char)));
    strncpy(dest_file, source_file, strlen(source_file)-2);
    dest_file[strlen(source_file)-2] = '.';
    dest_file[strlen(source_file)-1] = 's';
    dest_file[strlen(source_file)] = '\0';

    printf("%s\n", dest_file);

    FILE *file = fopen(dest_file, "w");
    if (file == NULL) {
        panic("Error opening file");
    }

    program_emit(file, program_asm);

    fclose(file);
    */

    return 0;
}