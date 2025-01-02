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

int is_c_file(char* file_name) {
    char* dot = strrchr(file_name, '.');
    return (dot && !strcmp(dot, ".c"));
}

int main(int argc, char** argv) {
    if (argc < 2) {
        panic("not enough arguments. usage: ./ecc [-c] <path.c>... [--lex, --parse, --codegen]");
    }

    list(char*)* files = list_init();
    list(char*)* asm_files = list_init();

    bool lex = false, parse = false, tacky = false, codegen = false;
    int object_file = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "./ecc") == 0) {
            continue;
        } else if (is_c_file(argv[i])) {
            list_append(files, (void*)argv[i]);
        } else if (strcmp(argv[i], "-c") == 0) {
            object_file = 1;
        } else if (strcmp(argv[i], "--lex")) {
            lex = true;
        } else if (strcmp(argv[i], "--parse")) {
            parse = true;
        } else if (strcmp(argv[i], "--tacky")) {
            tacky = true;
        } else if (strcmp(argv[i], "--codegen")) {
            codegen = true;
        }
    }
    
    for (int i = 0; i < files->len; i++) {
        char* source_file = (char*)list_get(files, i);

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

        list_append(asm_files, (void*)dest_file);

        printf("%s\n", dest_file);

        FILE *file = fopen(dest_file, "w");
        if (file == NULL) {
            panic("Error opening file");
        }

        emit_program(file, program_asm);

        fclose(file);

    }

    char* first_file = (char*)list_get(files, 0);
    char* executable_dest = strdup(first_file);
    if (!object_file) {
        executable_dest[strlen(executable_dest)-2] = '\0';
    } else {
        executable_dest[strlen(executable_dest)-1] = 'o';
    }
    

    char command[1024] = "gcc -o ";
    strcat(command, executable_dest);
    strcat(command, " ");
    if (object_file)
        strcat(command, "-c ");
    
    for (int i = 0; i < asm_files->len; i++) {
        char* asm_file = (char*)list_get(asm_files, i);
        strcat(command, asm_file);
        strcat(command, " ");
    }

    printf("%s\n", command);
    if (system(command) == -1) {
        panic("gcc unable to be invoked...");
    }

    printf("Successfully compiled file!\n");

    return 0;
}