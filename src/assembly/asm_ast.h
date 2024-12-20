#include "../utils/utils_list.h"
#include "../parser/parser.h"
#include "../ir/ir.h"

#pragma once

// for typing and documentation purposes
#define list(t) list

typedef struct
{
    int i;
} asm_immediate_node;

typedef enum {
    AX,
    R10,
} asm_register_t;

typedef struct
{
    asm_register_t reg;
} asm_register_node;

typedef struct {
    char* reg_name;
} asm_pseudo_node;

typedef struct {
    int offset;
} asm_stack_node;

typedef enum
{
    REG,
    IMM,
    PSEUDO,
    STACK,
} operand_type;

typedef struct
{
    operand_type type;
    union
    {
        asm_register_node* reg;
        asm_immediate_node* imm;
        asm_pseudo_node* pseudo;
        asm_stack_node* stack;
    } operand;
} operand_node;

typedef struct
{
    operand_node* src;
    operand_node* dest;
} asm_move_node;

typedef struct {
    int n_bytes;
} asm_stackalloc_node;

typedef enum {
    ASM_NOT,
    ASM_NEG,
} asm_unary_op;

typedef struct {
    asm_unary_op op;
    operand_node* dest;
} asm_unary_node;

typedef enum
{
    INSTR_MOV,
    INSTR_RET,
    INSTR_STACKALLOC,
    INSTR_UNARY,
} instruction_type;

typedef struct
{
    instruction_type type;
    union
    {
        asm_move_node* mov;
        asm_stackalloc_node* stackalloc;
        asm_unary_node* unary;
    } instruction;
} asm_instruction_node;

typedef struct asm_function_node
{
    char *identifier;
    list(asm_instruction_node*)* instructions;
} asm_function_node;

typedef struct asm_program_node
{
    asm_function_node *function;
} asm_program_node;

asm_program_node* ir_program_to_asm(ir_program_node* program);
asm_function_node* ir_function_to_asm(ir_function_node* function);

// instructions:
list(asm_instruction_node*)* ir_unary_to_asm(ir_unary_node* stmt);
list(asm_instruction_node*)* ir_return_to_asm(ir_return_node* ret);