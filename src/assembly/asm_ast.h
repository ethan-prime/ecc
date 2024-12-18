#include "../utils/utils_list.h"
#include "../parser/parser.h"

#pragma once

// for typing and documentation purposes
#define list(t) list

typedef struct
{
    int i;
} asm_immediate_node;

typedef struct
{
    char *reg_name;
} asm_register_node;

typedef enum
{
    REG,
    IMM,
} operand_type;

typedef struct
{
    operand_type type;
    union
    {
        asm_register_node reg;
        asm_immediate_node imm;
    } operand;
} operand_node;

typedef struct
{
    operand_node src;
    operand_node dest;
} asm_move_node;

typedef struct asm_ret_node
{
} asm_return_node;

typedef enum
{
    INST_MOV,
    INST_RET,
} instruction_type;

typedef struct
{
    instruction_type type;
    union
    {
        asm_move_node mov;
        asm_return_node ret;
    } instruction;
} asm_instruction_node;

typedef struct asm_function_node
{
    char *identifier;
    list(asm_instruction_node *) * instructions;
} asm_function_node;

typedef struct asm_program_node
{
    asm_function_node *function;
} asm_program_node;

asm_program_node *program_to_asm(program_node *program);
asm_function_node *function_to_asm(function_node *function);
list(asm_instruction_node *) * statement_to_asm(statement_node *stmt);

// STATEMENTS:
list(asm_instruction_node *) * return_to_asm(return_node *ret);