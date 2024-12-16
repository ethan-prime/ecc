#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
    return 0;
}