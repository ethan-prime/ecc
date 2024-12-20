#include "../utils/utils_list.h"
#include "asm_ast.h"
#include "../parser/parser.h"
#include "../utils/stackmap.h"

#pragma once

// replace pseudo stores with stack stores...
void replace_pseudo_stack(stackmap* sm, operand_node* pseudo_node);
void replace_pseudo_function(asm_function_node* function);
void replace_pseudo_pass(asm_program_node* program);