// WE NEED TO USE UNIONS TO SUPPORT MULTIPLE TYPES...
// RIGHT NOW WE ARE ONLY SUPPORTING ONE
#pragma once

typedef struct constant_node {
    int val;
} constant_node;

typedef enum {
    COMPLEMENT,
    NEGATE,
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
    REMAINDER
} binary_op;

typedef struct {
    binary_op op;
    struct expr_node* lhs;
    struct expr_node* rhs;
} binary_node;

typedef enum {
    EXPR_CONSTANT,
    EXPR_UNARY,
    EXPR_BINARY,
} expr_type;

typedef struct expr_node {
    expr_type type;
    union {
        constant_node* constant;
        unary_node* unary_expr;
        binary_node* binary_expr;
    } expr;
} expr_node;

typedef struct return_node {
    expr_node* expr;
} return_node;

typedef struct statement_node {
    return_node* ret;
} statement_node;

typedef struct function_node {
    char* identifier;
    statement_node* body;
} function_node;

typedef struct program_node {
    function_node* function;
} program_node;

// pretty-prints the ast of the program.
void print_ast(program_node* program);