// This represnts our Three-Adress Code IR.
#include "../utils/utils_list.h"
#include "../parser/parser.h"

#pragma once

typedef struct
{
    int i;
} ir_constant_node;

typedef struct
{
    char *identifier;
} ir_var_node;

typedef enum
{
    IR_CONSTANT,
    IR_VAR,
} ir_val_type;

typedef struct
{
    ir_val_type type;
    union
    {
        ir_constant_node* constant;
        ir_var_node* var;
    } val;
} ir_val_node;

typedef enum {
    IR_COMPLEMENT,
    IR_NEGATE,
    IR_NOT,
} ir_unary_op;

typedef struct
{
    ir_unary_op op;
    ir_val_node* src;
    ir_val_node* dest;
} ir_unary_node;

typedef enum {
    IR_ADD,
    IR_SUBTRACT,
    IR_MULTIPLY,
    IR_DIVIDE,
    IR_REMAINDER,
    IR_BITWISE_AND,
    IR_BITWISE_OR,
    IR_BITWISE_XOR,
    IR_BITWISE_LEFT_SHIFT,
    IR_BITWISE_RIGHT_SHIFT,
    IR_LOGICAL_LT,
    IR_LOGICAL_LTE,
    IR_LOGICAL_GT,
    IR_LOGICAL_GTE,
    IR_LOGICAL_EQUAL,
    IR_LOGICAL_NOT_EQUAL,
    IR_LOGICAL_AND,
    IR_LOGICAL_OR,
} ir_binary_op;

typedef struct {
    ir_binary_op op;
    ir_val_node* src1;
    ir_val_node* src2;
    ir_val_node* dest;
} ir_binary_node;

typedef struct {
    ir_val_node* val;
} ir_return_node;

typedef struct {
    ir_val_node* src;
    ir_val_node* dest;
} ir_copy_node;

typedef struct {
    char* target;
} ir_jump_node;

typedef struct {
    ir_val_node* condition;
    char* target;
} ir_jumpz_node;

typedef struct {
    ir_val_node* condition;
    char* target;
} ir_jumpnz_node;

typedef struct {
    char* identifier;
} ir_label_node;

typedef enum
{
    IR_INSTR_RET,
    IR_INSTR_UNARY,
    IR_INSTR_BINARY,
    IR_INSTR_COPY,
    IR_INSTR_JUMP,
    IR_INSTR_JUMPZ,
    IR_INSTR_JUMPNZ,
    IR_INSTR_LABEL,
} ir_instruction_type;

typedef struct
{
    ir_instruction_type type;
    union
    {
        ir_return_node* ret;
        ir_unary_node* unary;
        ir_binary_node* binary;
        ir_copy_node* copy;
        ir_jump_node* jump;
        ir_jumpz_node* jumpz;
        ir_jumpnz_node* jumpnz;
        ir_label_node* label;
    } instruction;
} ir_instruction_node;

typedef struct ir_function_node
{
    char* identifier;
    list(ir_instruction_node*)* instructions;
} ir_function_node;

typedef struct ir_program_node
{
    ir_function_node* function;
} ir_program_node;

// used for creating temperorary variables
char* ir_make_temp_var();

ir_program_node* program_to_ir(program_node *program);
ir_function_node* function_to_ir(function_node *function);

list(ir_instruction_node*)* declaration_to_ir(declaration_node* declare);

// STATEMENTS:
list(ir_instruction_node *)* statement_to_ir(statement_node *stmt);
void return_to_ir(return_node *ret, list(ir_instruction_node*)* instrs);
// appends instructions to generate expr -> ir, returns ir_val_node with DEST ir val.
ir_val_node* assign_to_ir(assign_node* assign, list(ir_instruction_node *)* instructions);
ir_val_node* expr_to_ir(expr_node* expr, list(ir_instruction_node *)* instructions);

list(ir_instruction_node*)* block_item_to_ir(block_item_node* block_item);

// prints TAC AST.
void ir_print_program(ir_program_node* program_ir);

char* ir_make_n_ident(char* prefix, int n);