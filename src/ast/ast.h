// WE NEED TO USE UNIONS TO SUPPORT MULTIPLE TYPES...
// RIGHT NOW WE ARE ONLY SUPPORTING ONE
#pragma once
#include "../utils/utils_list.h"

typedef struct constant_node {
    int val;
} constant_node;

typedef enum {
    COMPLEMENT,
    NEGATE,
    NOT, //(!)
} unary_op;

struct expr_node;
typedef struct {
    unary_op op;
    struct expr_node* expr;
} unary_node;

typedef enum {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    REMAINDER,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_LEFT_SHIFT,
    BITWISE_RIGHT_SHIFT,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_EQUAL,
    LOGICAL_NOT_EQUAL,
    LOGICAL_LT,
    LOGICAL_GT,
    LOGICAL_LTE,
    LOGICAL_GTE,
} binary_op;

typedef struct {
    binary_op op;
    struct expr_node* lhs;
    struct expr_node* rhs;
} binary_node;

typedef struct {
    char* identifier;
} variable_node;

typedef struct {
    struct expr_node* lvalue;
    struct expr_node* expr;
} assign_node;

typedef enum {
    EXPR_CONSTANT,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
} expr_type;

typedef struct expr_node {
    expr_type type;
    union {
        constant_node* constant;
        unary_node* unary_expr;
        binary_node* binary_expr;
        variable_node* variable;
        assign_node* assign;
    } expr;
} expr_node;

typedef struct return_node {
    expr_node* expr;
} return_node;

typedef enum {
    STMT_RET,
    STMT_EXPR,
    STMT_NULL,
} statement_type;

typedef struct statement_node {
    statement_type type;
    union {
        return_node* ret;
        expr_node* expr;
    } stmt;
} statement_node;

typedef struct {
    char* identifier;
    expr_node* init; // optional
} declaration_node;

typedef enum {
    BLOCK_DECLARE,
    BLOCK_STMT,
} block_type;

typedef struct {
    block_type type;
    union {
        declaration_node* declare;
        statement_node* stmt;
    } item; 
} block_item_node;

typedef struct function_node {
    char* identifier;
    list(block_item_node*)* body;
} function_node;

typedef struct program_node {
    function_node* function;
} program_node;

// pretty-prints the ast of the program.
void print_ast(program_node* program);