#include <stdio.h>
#include "../assembly/asm_ast.h"
#include "../utils/utils_list.h"

#pragma once

void emit_function(FILE* file, asm_function_node* function);
void emit_program(FILE* file, asm_program_node* program);