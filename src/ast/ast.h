// WE NEED TO USE UNIONS TO SUPPORT MULTIPLE TYPES...
// RIGHT NOW WE ARE ONLY SUPPORTING ONE

typedef struct constant_node {
    int val;
} constant_node;

typedef struct expr_node {
    constant_node* constant;
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