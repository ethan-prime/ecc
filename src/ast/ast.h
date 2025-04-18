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
    PREFIX_INC, // ++a
    POSTFIX_INC, // a++
    PREFIX_DEC, // --a
    POSTFIX_DEC, // a--
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

typedef struct {
    struct expr_node* condition;
    struct expr_node* condition_true;
    struct expr_node* condition_false;
} ternary_node;

typedef struct {
    char* identifier;
    list(struct expr_node*)* args;
} function_call_node;

typedef enum {
    EXPR_CONSTANT,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_TERNARY,
    EXPR_FUNCTION_CALL,
} expr_type;

typedef struct expr_node {
    expr_type type;
    union {
        constant_node* constant;
        unary_node* unary_expr;
        binary_node* binary_expr;
        variable_node* variable;
        assign_node* assign;
        ternary_node* ternary;
        function_call_node* function_call;
    } expr;
} expr_node;

typedef struct {
    char* identifier;
    expr_node* init; // optional
} variable_declaration_node;

typedef struct {
    char* identifier;
} param_node;

struct block_node;
typedef struct {
    char* name;
    list(param_node*)* params;
    struct block_node* body;
} function_declaration_node;

typedef enum {
    DECLARE_VARIABLE,
    DECLARE_FUNCTION,
} declaration_type;

typedef struct {
    declaration_type type;
    union {
        variable_declaration_node* variable;
        function_declaration_node* function;
    } declaration;
} declaration_node;

typedef struct return_node {
    expr_node* expr;
} return_node;

struct statement_node;
typedef struct if_node {
    expr_node* condition;
    struct statement_node* then_stmt;
    struct statement_node* else_stmt; // optional
} if_node;

struct block_node;
typedef struct {
    struct block_node* block;
} compound_node;

typedef struct {
    char* loop_label;
} break_node;

typedef struct {
    char* loop_label;
} continue_node;

typedef struct {
    expr_node* condition;
    struct statement_node* body;
    char* loop_label;
} while_node;

typedef struct {
    struct statement_node* body;
    expr_node* condition;
    char* loop_label;
} do_while_node;

typedef enum {
    INIT_DECL,
    INIT_EXPR,
} for_init_type;

typedef struct {
    for_init_type type;
    union {
        variable_declaration_node* init_declare;
        expr_node* init_expr;
    } for_init;
} for_init_node;

typedef struct {
    for_init_node* init;
    expr_node* condition;
    expr_node* final_expr;
    struct statement_node* body;
    char* loop_label;
} for_node;

typedef enum {
    STMT_RET,
    STMT_EXPR,
    STMT_NULL,
    STMT_IF,
    STMT_COMPOUND,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_WHILE,
    STMT_DO_WHILE,
    STMT_FOR,
} statement_type;

typedef struct statement_node {
    statement_type type;
    union {
        return_node* ret;
        expr_node* expr;
        if_node* if_stmt;
        compound_node* compound;
        break_node* break_node;
        continue_node* continue_node;
        while_node* while_node;
        do_while_node* do_while_node;
        for_node* for_node;
    } stmt;
} statement_node;

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

typedef struct block_node {
    list(block_item_node*)* items;
} block_node;

typedef struct program_node {
    list(function_declaration_node*)* functions;
} program_node;

// pretty-prints the ast of the program.
void print_ast(program_node* program);
void print_statement(statement_node* stmt, int depth);
void print_expr(expr_node* expr, int depth);
void print_block(block_node* block, int depth);
void print_block_item(block_item_node* block_item, int depth);
void print_function_declare(function_declaration_node* decl, int depth);
void print_variable_declare(variable_declaration_node* decl, int depth);
void print_declare(declaration_node* decl, int depth);