#include "../config.h"
#if CODEGEN_ENABLE
#include <stdio.h>
#include "../assembly/asm_ast.h"
#include "../utils/utils_list.h"
#include "../utils/symboltable.h"

#pragma once

void emit_function(FILE* file, asm_function_node* function, symboltable* symbols);
void emit_program(FILE* file, asm_program_node* program, symboltable* symbols);
#endif