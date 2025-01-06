#include "../config.h"
#if ASM_ENABLE
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

typedef enum
{
    AX,
    CX,
    DX,
    DI,
    SI,
    R8,
    R9,
    R10,
    R11,
} asm_register_t;

typedef struct
{
    asm_register_t reg;
} asm_register_node;

typedef struct
{
    char *reg_name;
} asm_pseudo_node;

typedef struct
{
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
        asm_register_node *reg;
        asm_immediate_node *imm;
        asm_pseudo_node *pseudo;
        asm_stack_node *stack;
    } operand;
} operand_node;

typedef enum
{
    OP_8_BYTES,
    OP_4_BYTES,
    OP_1_BYTE,
} op_size;

typedef struct
{
    operand_node *src;
    operand_node *dest;
    op_size size;
} asm_move_node;

typedef struct
{
    int n_bytes;
} asm_stackalloc_node;

typedef struct {
    int n_bytes;
} asm_stackdealloc_node;

typedef enum
{
    ASM_NOT,
    ASM_NEG,
} asm_unary_op;

typedef struct
{
    asm_unary_op op;
    operand_node *dest;
} asm_unary_node;

typedef enum
{
    ASM_ADD,
    ASM_SUB,
    ASM_MULT,
    ASM_AND,
    ASM_OR,
    ASM_XOR,
    ASM_LEFT_SHIFT,
    ASM_RIGHT_SHIFT,
} asm_binary_op;

typedef struct
{
    asm_binary_op op;
    operand_node *src;
    operand_node *dest;
} asm_binary_node;

typedef struct
{
    operand_node *divisor;
} asm_idiv_node;

typedef struct
{
    operand_node *src1;
    operand_node *src2;
} asm_cmp_node;

typedef struct
{
    char *identifier;
} asm_jmp_node;

typedef enum
{
    E,
    NE,
    G,
    GE,
    L,
    LE
} cond_code;

typedef struct
{
    cond_code type;
    char *identifier;
} asm_jmpcc_node;

typedef struct
{
    cond_code type;
    operand_node *dest;
} asm_setcc_node;

typedef struct
{
    char *identifier;
} asm_label_node;

typedef struct {
    operand_node* op;
} asm_push_node;

typedef struct {
    char* identifier;
} asm_call_node;

typedef enum
{
    INSTR_MOV,
    INSTR_RET,
    INSTR_STACKALLOC,
    INSTR_STACKDEALLOC,
    INSTR_UNARY,
    INSTR_BINARY,
    INSTR_IDIV,
    INSTR_CDQ,
    INSTR_CMP,
    INSTR_JMP,
    INSTR_JMPCC,
    INSTR_SETCC,
    INSTR_LABEL,
    INSTR_PUSH,
    INSTR_CALL,
} instruction_type;

typedef struct
{
    instruction_type type;
    union
    {
        asm_move_node *mov;
        asm_stackalloc_node *stackalloc;
        asm_stackdealloc_node *stackdealloc;
        asm_unary_node *unary;
        asm_binary_node *binary;
        asm_idiv_node *idiv;
        asm_cmp_node *cmp;
        asm_jmp_node *jmp;
        asm_jmpcc_node *jmpcc;
        asm_setcc_node *setcc;
        asm_label_node *label;
        asm_push_node* push;
        asm_call_node* call;
    } instruction;
} asm_instruction_node;

typedef struct asm_function_node
{
    char *identifier;
    list(asm_instruction_node *) * instructions;
    stackmap *sm;
} asm_function_node;

typedef struct asm_program_node
{
    list(asm_function_node*)* functions;
} asm_program_node;

typedef asm_instruction_node asm_i;

asm_program_node *ir_program_to_asm(ir_program_node *program);
asm_function_node *ir_function_to_asm(ir_function_node *function);

// instructions:
list(asm_i *) * ir_unary_to_asm(ir_unary_node *unary);
list(asm_i *) * ir_return_to_asm(ir_return_node *ret);

list(asm_i *) * ir_binary_to_asm(ir_binary_node *binary);
list(asm_i *) * ir_relational_to_asm(ir_binary_node *binary);
list(asm_i *) * ir_shift_to_asm(ir_binary_node *binary);
list(asm_i *) * ir_remainder_to_asm(ir_binary_node *binary);
list(asm_i *) * ir_div_to_asm(ir_binary_node *binary);

list(asm_i *) * ir_jump_to_asm(ir_jump_node *jump);
list(asm_i *) * ir_jumpz_to_asm(ir_jumpz_node *jumpz);
list(asm_i *) * ir_jumpnz_to_asm(ir_jumpnz_node *jumpnz);

asm_i *asm_create_idiv_instr(operand_node *divisor);
asm_i *asm_create_mov_instr(op_size size, operand_node *src, operand_node *dest);
asm_i *asm_create_unary_instr(asm_unary_op op, operand_node *dest);
asm_i *asm_create_binary_instr(asm_binary_op op, operand_node *src, operand_node *dest);
asm_i *asm_create_ret_instr();
asm_i *asm_create_cdq_instr();
asm_i *asm_create_jmp_instr(char *identifier);
asm_i *asm_create_jmpcc_instr(cond_code type, char *identifier);
asm_i *asm_create_cmp_instr(operand_node *src1, operand_node *src2);
asm_i *asm_create_setcc_instr(cond_code type, operand_node *dest);
asm_i *asm_create_stackalloc_instr(int size);
asm_i* asm_create_stackdealloc_instr(int size);
asm_i* asm_create_push_instr(operand_node* op);
asm_i* asm_create_call_instr(char* identifier);

operand_node *asm_create_register_operand(asm_register_t reg);
asm_pseudo_node *asm_create_pseudo(char *identifier);
asm_immediate_node *asm_create_immediate(int n);

#define IMMEDIATE(n) (asm_create_immediate_operand(n))
#define REGISTER(r) (asm_create_register_operand(r))
#endif