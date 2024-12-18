#include <stdio.h>
#include "../assembly/asm_ast.h"
#include "../utils/utils_list.h"

#pragma once

void program_emit(FILE* file, asm_program_node* program);
void function_emit(FILE* file, asm_function_node* function);
void instruction_emit(FILE* file, asm_instruction_node* instr);
void instructions_emit(FILE* file, list(asm_instruction_node*)* instrs);
void operand_emit(FILE* file, operand_node* operand);

void mov_emit(FILE* file, asm_move_node* mov);
void ret_emit(FILE* file);