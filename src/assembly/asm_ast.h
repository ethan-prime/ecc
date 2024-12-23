#include "../utils/utils_list.h"
#include "../parser/parser.h"
#include "../ir/ir.h"
#include "../utils/stackmap.h"

#pragma once

// for typing and documentation purposes
#define list(t) list

typedef struct
{
    int i;
} asm_immediate_node;

typedef enum {
    AX,
    DX,
    R10,
    R11,
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

typedef enum {
    ASM_ADD,
    ASM_SUB,
    ASM_MULT,
    ASM_AND,
    ASM_OR,
    ASM_XOR,
    ASM_LEFT_SHIFT,
    ASM_RIGHT_SHIFT,
} asm_binary_op;

typedef struct {
    asm_binary_op op;
    operand_node* src;
    operand_node* dest;
} asm_binary_node;

typedef struct {
    operand_node* divisor;
} asm_idiv_node;

typedef enum
{
    INSTR_MOV,
    INSTR_RET,
    INSTR_STACKALLOC,
    INSTR_UNARY,
    INSTR_BINARY,
    INSTR_IDIV,
    INSTR_CDQ,
} instruction_type;

typedef struct
{
    instruction_type type;
    union
    {
        asm_move_node* mov;
        asm_stackalloc_node* stackalloc;
        asm_unary_node* unary;
        asm_binary_node* binary;
        asm_idiv_node* idiv;
    } instruction;
} asm_instruction_node;

typedef struct asm_function_node
{
    char *identifier;
    list(asm_instruction_node*)* instructions;
    stackmap* sm;
} asm_function_node;

typedef struct asm_program_node
{
    asm_function_node *function;
} asm_program_node;

typedef asm_instruction_node asm_i;

asm_program_node* ir_program_to_asm(ir_program_node* program);
asm_function_node* ir_function_to_asm(ir_function_node* function);

// instructions:
list(asm_i*)* ir_unary_to_asm(ir_unary_node* unary);
list(asm_i*)* ir_binary_to_asm(ir_binary_node* binary);
list(asm_i*)* ir_return_to_asm(ir_return_node* ret);

asm_i* asm_create_idiv_instr(operand_node* divisor);
asm_i* asm_create_move_instr(operand_node* src, operand_node* dest);
asm_i* asm_create_unary_instr(asm_unary_op op, operand_node* dest);
asm_i* asm_create_binary_instr(asm_binary_op op, operand_node* src, operand_node* dest);
asm_i* asm_create_ret_instr();
asm_i* asm_create_cdq_instr();

operand_node* asm_create_register_operand(asm_register_t reg);
#define REGISTER(r) (asm_create_register_operand(r))
asm_pseudo_node* asm_create_pseudo(char* identifier);
asm_immediate_node* asm_create_immediate(int n);
#define IMMEDIATE(n) (asm_create_immediate(n))
