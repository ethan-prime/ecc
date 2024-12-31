#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer/lexer.h"
#include "lexer/tokens.h"
#include "parser/parser.h"
#include "semantics/semantics.h"
#include "assembly/asm_ast.h"
#include "assembly/passes.h"
#include "codegen/codegen.h"
#include "ir/ir.h"

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

    bool lex = false, parse = false, tacky = false, codegen = false;
    
    if (argc > 2) {
        if (strcmp(stage, "--lex") == 0) {
            lex = true;
        } else if (strcmp(stage, "--parse") == 0) {
            parse = true;
        } else if (strcmp(stage, "--codegen") == 0) {
            codegen = true;
        } else if (strcmp(stage, "--tacky") == 0) {
            tacky = true;
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

    // semantic pass
    resolve_program(program);
    label_program(program);

    printf("Parsed program:\n");
    print_ast(program);

    if (parse) {
        exit(0);
    }

    ir_program_node* program_ir = program_to_ir(program);

    printf("Succesfully codegened to TAC...\n");
    ir_print_program(program_ir);

    if (tacky) {
        exit(0);
    }

    asm_program_node* program_asm = ir_program_to_asm(program_ir);
    replace_pseudo_pass(program_asm);
    pass2(program_asm);

    printf("Succesfully codegened to ASM...\n");

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

    emit_program(file, program_asm);

    fclose(file);

    printf("Successfully compiled file!\n");

    return 0;
}