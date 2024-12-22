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

typedef enum
{
    IR_INSTR_RET,
    IR_INSTR_UNARY,
} ir_instruction_type;

typedef struct
{
    ir_instruction_type type;
    union
    {
        ir_return_node* ret;
        ir_unary_node* unary;
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

// appends instructions to generate expr -> ir, returns ir_val_node with DEST ir val.
ir_val_node* expr_to_ir(expr_node* expr, list(ir_instruction_node *)* instructions);

ir_program_node* program_to_ir(program_node *program);
ir_function_node* function_to_ir(function_node *function);
list(ir_instruction_node *)* statement_to_ir(statement_node *stmt);

// STATEMENTS:
list(ir_instruction_node *)* return_to_ir(return_node *ret);

// prints TAC AST.
void ir_print_program(ir_program_node* program_ir);