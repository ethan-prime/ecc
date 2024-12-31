#include "../utils/tokenqueue.h"
#include "../lexer/tokens.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

// returns 1 if head matches token_id, 0 otherwise
int expect(token_queue *tq, token_id id)
{
    return (CUR(tq)->id == id) ? 1 : 0;
}

// prints a parser error and quits the program
void parser_error(char *expected, token *received)
{
    printf("*** ecc: syntax error ***\n");
    printf("expected: %s\n", expected);
    printf("received: ");
    print_token(*received);
    printf("@ line %d\n", received->line_number);
    exit(1);
}

// parses and returns corresponding ast nodes
constant_node *parse_constant(token_queue *tq)
{
    if (!expect(tq, CONSTANT))
    {
        parser_error("a constant literal", CUR(tq));
    };

    constant_node *node = MALLOC(constant_node);
    node->val = CUR(tq)->val.i;
    DEQ(tq);

    return node;
}

expr_node *create_constant_expr(int c)
{
    constant_node *node = MALLOC(constant_node);
    node->val = c;

    expr_node *expr = MALLOC(expr_node);
    expr->type = EXPR_CONSTANT;
    expr->expr.constant = node;

    return expr;
}

int is_unary_op(token *tok)
{
    return (tok->id == HYPHEN || tok->id == TILDE || tok->id == EXCLAM);
}

unary_node *parse_unary_expr(token_queue *tq)
{
    unary_node *node = MALLOC(unary_node);

    if (!is_unary_op(CUR(tq)))
    {
        parser_error("a unary operator", CUR(tq));
    }

    token *cur = CUR(tq);

    if (cur->id == HYPHEN)
    {
        node->op = NEGATE;
    }
    else if (cur->id == TILDE)
    {
        node->op = COMPLEMENT;
    }
    else if (cur->id == EXCLAM)
    {
        node->op = NOT;
    }

    DEQ(tq);

    node->expr = parse_factor(tq);
    return node;
}

// creates and returns a binary expr node.
expr_node *create_binary_expr(binary_op op, expr_node *lhs, expr_node *rhs)
{
    expr_node *node = MALLOC(expr_node);
    node->type = EXPR_BINARY;

    binary_node *binary_expr = MALLOC(binary_node);
    binary_expr->lhs = lhs;
    binary_expr->rhs = rhs;
    binary_expr->op = op;

    node->expr.binary_expr = binary_expr;

    return node;
}

expr_node *create_assign(expr_node *lhs, expr_node *rhs)
{
    expr_node *node = MALLOC(expr_node);
    node->type = EXPR_ASSIGN;

    assign_node *assign = MALLOC(assign_node);
    assign->expr = rhs;
    assign->lvalue = lhs;

    node->expr.assign = assign;

    return node;
}

expr_node* create_ternary_expr(expr_node* lhs, expr_node* middle, expr_node* rhs) {
    expr_node* node = MALLOC(expr_node);
    node->type = EXPR_TERNARY;

    ternary_node* ternary = MALLOC(ternary_node);
    ternary->condition = lhs;
    ternary->condition_true =  middle;
    ternary->condition_false = rhs;

    node->expr.ternary = ternary;

    return node;
}

variable_node *parse_variable_node(token_queue *tq)
{
    variable_node *var = MALLOC(variable_node);

    if (!expect(tq, IDENTIFIER))
    {
        parser_error("an identifier", CUR(tq));
    }

    var->identifier = CUR(tq)->val.s;

    DEQ(tq);

    return var;
}

expr_node *parse_factor(token_queue *tq)
{
    expr_node *node = MALLOC(expr_node);

    token *cur = CUR(tq);

    if (cur->id == CONSTANT)
    {
        node->type = EXPR_CONSTANT;
        node->expr.constant = parse_constant(tq);
    }
    else if (cur->id == OPEN_PAREN)
    {
        DEQ(tq);
        node = parse_expr(tq, 0); // start with a precedence of 0, include any operators
        if (!expect(tq, CLOSE_PAREN))
        {
            parser_error(")", CUR(tq));
        }
        DEQ(tq);
        return node;
    }
    else if (is_unary_op(CUR(tq)))
    {
        node->type = EXPR_UNARY;
        node->expr.unary_expr = parse_unary_expr(tq);
    }
    else if (CUR(tq)->id == IDENTIFIER)
    {
        node->type = EXPR_VARIABLE;
        node->expr.variable = parse_variable_node(tq);
    }

    return node;
}

int is_prefix_postfix_op(token *tok)
{
    return (tok->id == PLUS_PLUS || tok->id == HYPHEN_HYPHEN);
}

// returns 1 if the token is a compound assignment token,
// such as +=, -=, *=, &=, etc. sets *op to its corresponding operator.
int is_compound_assign(token *tok, binary_op *op)
{
    token_id id = tok->id;
    if (id == PLUS_EQUAL)
    {
        *op = ADD;
        return 1;
    }
    else if (id == HYPHEN_EQUAL)
    {
        *op = SUBTRACT;
        return 1;
    }
    else if (id == ASTERISK_EQUAL)
    {
        *op = MULTIPLY;
        return 1;
    }
    else if (id == FORWARD_SLASH_EQUAL)
    {
        *op = DIVIDE;
        return 1;
    }
    else if (id == PERCENT_EQUAL)
    {
        *op = REMAINDER;
        return 1;
    }
    else if (id == AMPERSAND_EQUAL)
    {
        *op = BITWISE_AND;
        return 1;
    }
    else if (id == PIPE_EQUAL)
    {
        *op = BITWISE_OR;
        return 1;
    }
    else if (id == CARAT_EQUAL)
    {
        *op = BITWISE_XOR;
        return 1;
    }
    else if (id == LEFT_SHIFT_EQUAL)
    {
        *op = BITWISE_LEFT_SHIFT;
        return 1;
    }
    else if (id == RIGHT_SHIFT_EQUAL)
    {
        *op = BITWISE_RIGHT_SHIFT;
        return 1;
    }
    return 0;
}

int is_binary_op(token *tok)
{
    token_id id = tok->id;
    if (id == PLUS || id == HYPHEN || id == ASTERISK || id == FORWARD_SLASH || id == PERCENT || id == PERCENT || id == AMPERSAND || id == PIPE || id == CARAT || id == LEFT_SHIFT || id == RIGHT_SHIFT || id == AMPERSAND_AMPERSAND || id == PIPE_PIPE || id == EQUAL_EQUAL || id == EXCLAM_EQUAL || id == LESS_THAN || id == LESS_THAN_EQUAL || id == GREATER_THAN || id == GREATER_THAN_EQUAL || id == EQUAL || id == PLUS_EQUAL || id == HYPHEN_EQUAL || id == ASTERISK_EQUAL || id == FORWARD_SLASH_EQUAL || id == PERCENT_EQUAL || id == AMPERSAND_EQUAL || id == PIPE_EQUAL || id == CARAT_EQUAL || id == LEFT_SHIFT_EQUAL || id == RIGHT_SHIFT_EQUAL || id == QUESTION)
    {
        return 1;
    }
    return 0;
}

binary_op parse_binop(token_queue *tq)
{
    binary_op op;
    switch (CUR(tq)->id)
    {
    case PLUS:
        op = ADD;
        break;
    case HYPHEN:
        op = SUBTRACT;
        break;
    case ASTERISK:
        op = MULTIPLY;
        break;
    case FORWARD_SLASH:
        op = DIVIDE;
        break;
    case PERCENT:
        op = REMAINDER;
        break;
    case AMPERSAND:
        op = BITWISE_AND;
        break;
    case AMPERSAND_AMPERSAND:
        op = LOGICAL_AND;
        break;
    case PIPE:
        op = BITWISE_OR;
        break;
    case PIPE_PIPE:
        op = LOGICAL_OR;
        break;
    case CARAT:
        op = BITWISE_XOR;
        break;
    case LEFT_SHIFT:
        op = BITWISE_LEFT_SHIFT;
        break;
    case RIGHT_SHIFT:
        op = BITWISE_RIGHT_SHIFT;
        break;
    case LESS_THAN:
        op = LOGICAL_LT;
        break;
    case GREATER_THAN:
        op = LOGICAL_GT;
        break;
    case LESS_THAN_EQUAL:
        op = LOGICAL_LTE;
        break;
    case GREATER_THAN_EQUAL:
        op = LOGICAL_GTE;
        break;
    case EQUAL_EQUAL:
        op = LOGICAL_EQUAL;
        break;
    case EXCLAM_EQUAL:
        op = LOGICAL_NOT_EQUAL;
        break;
    default:
        parser_error("a binary op", CUR(tq));
    }
    DEQ(tq);
    return op;
}

int precedence(token *tok)
{
    if (!is_binary_op(tok))
    {
        parser_error("an operator (when getting precedence)", tok);
    }
    switch (tok->id)
    {
    case PLUS:
        return PREC_ADD;
    case HYPHEN:
        return PREC_SUBTRACT;
    case ASTERISK:
        return PREC_MULTIPLY;
    case FORWARD_SLASH:
        return PREC_DIVIDE;
    case PERCENT:
        return PREC_REMAINDER;
    case AMPERSAND:
        return PREC_BITWISE_AND;
    case PIPE:
        return PREC_BITWISE_OR;
    case CARAT:
        return PREC_BITWISE_XOR;
    case LEFT_SHIFT:
        return PREC_LEFT_SHIFT;
    case RIGHT_SHIFT:
        return PREC_RIGHT_SHIFT;
    case AMPERSAND_AMPERSAND:
        return PREC_LOGICAL_AND;
    case PIPE_PIPE:
        return PREC_LOGICAL_OR;
    case EQUAL_EQUAL:
        return PREC_LOGICAL_EQUAL;
    case EXCLAM_EQUAL:
        return PREC_LOGICAL_NOT_EQUAL;
    case LESS_THAN:
        return PREC_LT;
    case LESS_THAN_EQUAL:
        return PREC_LTE;
    case GREATER_THAN:
        return PREC_GT;
    case GREATER_THAN_EQUAL:
        return PREC_GTE;
    case EQUAL:
        return PREC_ASSIGN;
    case PLUS_EQUAL:
        return PREC_ASSIGN;
    case HYPHEN_EQUAL:
        return PREC_ASSIGN;
    case ASTERISK_EQUAL:
        return PREC_ASSIGN;
    case FORWARD_SLASH_EQUAL:
        return PREC_ASSIGN;
    case PERCENT_EQUAL:
        return PREC_ASSIGN;
    case AMPERSAND_EQUAL:
        return PREC_ASSIGN;
    case PIPE_EQUAL:
        return PREC_ASSIGN;
    case CARAT_EQUAL:
        return PREC_ASSIGN;
    case LEFT_SHIFT_EQUAL:
        return PREC_ASSIGN;
    case RIGHT_SHIFT_EQUAL:
        return PREC_ASSIGN;
    case QUESTION:
        return PREC_TERNARY;
    default:
        return -1;
    }
}

assign_node *assign_node_copy(assign_node *assign)
{
    assign_node *node = MALLOC(assign_node);
    node->lvalue = expr_node_copy(assign->lvalue);
    node->expr = expr_node_copy(assign->expr);
    return node;
}

binary_node *binary_node_copy(binary_node *binary)
{
    binary_node *node = MALLOC(binary_node);
    node->lhs = expr_node_copy(binary->lhs);
    node->rhs = expr_node_copy(binary->rhs);
    node->op = binary->op;
    return node;
}

constant_node *constant_node_copy(constant_node *constant)
{
    constant_node *node = MALLOC(constant_node);
    node->val = constant->val;
    return node;
}

unary_node *unary_node_copy(unary_node *unary)
{
    unary_node *node = MALLOC(unary_node);
    node->op = unary->op;
    node->expr = expr_node_copy(unary->expr);
    return node;
}

variable_node *variable_node_copy(variable_node *var)
{
    variable_node *node = MALLOC(variable_node);
    node->identifier = var->identifier;
    return node;
}

// copies an expr node and returns a ptr to it.
// useful for parse_compound_assign, where lhs points
// to the same place in memory and semantic analysis will change
// a = a + 1 to a.0 = a.0 + 1 and then try to check if a.0 is defined, which of course it isnt.
// thats a temp var.
expr_node *expr_node_copy(expr_node *expr)
{
    expr_node *new_expr = MALLOC(expr_node);
    new_expr->type = expr->type;
    if (expr->type == EXPR_ASSIGN)
    {
        new_expr->expr.assign = assign_node_copy(expr->expr.assign);
    }
    else if (expr->type == EXPR_BINARY)
    {
        new_expr->expr.binary_expr = binary_node_copy(expr->expr.binary_expr);
    }
    else if (expr->type == EXPR_CONSTANT)
    {
        new_expr->expr.constant = constant_node_copy(expr->expr.constant);
    }
    else if (expr->type == EXPR_UNARY)
    {
        new_expr->expr.unary_expr = unary_node_copy(expr->expr.unary_expr);
    }
    else if (expr->type == EXPR_VARIABLE)
    {
        new_expr->expr.variable = variable_node_copy(expr->expr.variable);
    }
    return new_expr;
}

// ++<expr>
expr_node *parse_prefix(token_queue *tq)
{
    if (!is_prefix_postfix_op(CUR(tq)))
    {
        parser_error("a ++ or -- operator", CUR(tq));
    }
    binary_op op = (CUR(tq)->id == PLUS_PLUS) ? PREFIX_INC : PREFIX_DEC;
    DEQ(tq);
    expr_node *expr = parse_expr(tq, PREC_PREFIX);
    // remember, this is like (expr) = (expr <PREFIX_OP> 1)
    expr_node *binary = create_binary_expr(op, expr_node_copy(expr), create_constant_expr(1));
    expr_node *assign = create_assign(expr, binary);
    return assign;
}

// <expr>++
// the trick here is that, by this point, we will have just finished parsing an expr.
// by seeing the ++ right after, we know this must be a postfix operation.
// this differs from prefix as we will first encounter a prefix token before an expr is
// starting to be parsed. :)
expr_node *parse_postfix(token_queue *tq, expr_node *expr)
{
    if (!is_prefix_postfix_op(CUR(tq)))
    {
        parser_error("a ++ or -- operator", CUR(tq));
    }
    binary_op op = (CUR(tq)->id == PLUS_PLUS) ? POSTFIX_INC : POSTFIX_DEC;
    DEQ(tq);
    // remember, this is like (expr) = (expr <POSTFIX_OP> 1)
    expr_node *binary = create_binary_expr(op, expr_node_copy(expr), create_constant_expr(1));
    expr_node *assign = create_assign(expr, binary);
    return assign;
}

expr_node *parse_compound_assign(token_queue *tq, expr_node *lhs)
{
    // a += 1
    // a = a + 1
    // assign(lhs, parse_expr(lhs <op> rhs)
    binary_op op;
    if (!is_compound_assign(CUR(tq), &op))
    {
        parser_error("a coumpound assignment token", CUR(tq));
    }
    DEQ(tq);                                                                   // eat the operator
    expr_node *rhs = parse_expr(tq, PREC_ASSIGN);                              // get the rhs expr expr(1). right-to-left associativity, so just precedence + 0.
    expr_node *binary_expr = create_binary_expr(op, expr_node_copy(lhs), rhs); // expr(a + 1). we need a copy so semantics doesn't mess up...
    expr_node *assign = create_assign(lhs, binary_expr);                       // assign(expr(a), expr(a + 1))
    return assign;
}

// ? <expr> :
expr_node* parse_conditional_middle(token_queue *tq) {
    if (!expect(tq, QUESTION)) {
        parser_error("?", CUR(tq));
    }
    DEQ(tq);

    expr_node* node = parse_expr(tq, 0);

    if (!expect(tq, COLON)) {
        parser_error(":", CUR(tq));
    }
    DEQ(tq);

    return node;
}

// THIS FUNCTION IS TRASH I NEED TO CLEAN THIS UP LATER LOLLLLLL
// works for now, though.
expr_node *parse_expr(token_queue *tq, int min_precedence)
{
    expr_node *lhs;
    // we know this will be a prefix operation expression
    if (is_prefix_postfix_op(CUR(tq)))
    {
        lhs = parse_prefix(tq);
    }
    else
    {
        lhs = parse_factor(tq);
    }

    token *cur = CUR(tq);

binary_loop:
    while (is_binary_op(cur) && precedence(cur) >= min_precedence)
    {
        binary_op dummy; // we dont need this at all, just for function call.
        if (is_compound_assign(CUR(tq), &dummy))
        {
            return parse_compound_assign(tq, lhs);
        }
        if (CUR(tq)->id == EQUAL)
        {
            // we need to be right-associative
            DEQ(tq);
            expr_node *rhs = parse_expr(tq, precedence(cur));
            lhs = create_assign(lhs, rhs);
        } else if (CUR(tq)->id == QUESTION) {
            // ternary, right-associative
            expr_node* middle = parse_conditional_middle(tq);
            expr_node* rhs = parse_expr(tq, precedence(cur)); // right-associative
            lhs = create_ternary_expr(lhs, middle, rhs);
        }
        else
        {
            binary_op op = parse_binop(tq);
            expr_node *rhs = parse_expr(tq, precedence(cur) + 1);
            lhs = create_binary_expr(op, lhs, rhs);
        }
        cur = CUR(tq);
    }

    // we know this is a postfix operation expression!
    if (is_prefix_postfix_op(CUR(tq)))
    {
        lhs = parse_postfix(tq, lhs);
        cur = CUR(tq);
        goto binary_loop;
    }

    return lhs;
}

expr_node* parse_optional_expr(token_queue* tq, token_id end_token) {
    if (CUR(tq)->id == end_token) {
        DEQ(tq);
        return NULL;
    }
    expr_node* expr = parse_expr(tq, 0);
    if (!expect(tq, end_token)) {
        printf("%d\n", end_token);
        parser_error("end token for optional expr", CUR(tq));
    }
    DEQ(tq);

    return expr;
}

for_init_node* parse_for_init(token_queue* tq) {
    for_init_node* for_init = MALLOC(for_init_node);

    if (CUR(tq)->id == KEYW_INT) {
        // declaration
        for_init->type = INIT_DECL;
        for_init->for_init.init_declare = parse_declaration(tq);
    } else {
        // expression
        for_init->type = INIT_EXPR;
        for_init->for_init.init_expr = parse_optional_expr(tq, SEMICOLON);
    }

    return for_init;

}

break_node* parse_break(token_queue* tq) {
    break_node* node = MALLOC(break_node);

    if (!expect(tq, KEYW_BREAK)) {
        parser_error("break", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, SEMICOLON)) {
        parser_error(";", CUR(tq));
    }
    DEQ(tq);

    return node;
}

continue_node* parse_continue(token_queue* tq) {
    continue_node* node = MALLOC(continue_node);

    if (!expect(tq, KEYW_CONTINUE)) {
        parser_error("break", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, SEMICOLON)) {
        parser_error(";", CUR(tq));
    }
    DEQ(tq);

    return node;
}

while_node* parse_while(token_queue* tq) {
    if (!expect(tq, KEYW_WHILE)) {
        parser_error("while", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, OPEN_PAREN)) {
        parser_error("(", CUR(tq));
    }
    DEQ(tq);

    while_node* node = MALLOC(while_node);
    node->condition = parse_expr(tq, 0);

    if (!expect(tq, CLOSE_PAREN)) {
        parser_error(")", CUR(tq));
    }
    DEQ(tq);

    node->body = parse_statement(tq);

    return node;
}

do_while_node* parse_do_while(token_queue* tq) {
    do_while_node* node = MALLOC(do_while_node);

    if (!expect(tq, KEYW_DO)) {
        parser_error("do", CUR(tq));
    }
    DEQ(tq);

    node->body = parse_statement(tq);

    if (!expect(tq, KEYW_WHILE)) {
        parser_error("while", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, OPEN_PAREN)) {
        parser_error("(", CUR(tq));
    }
    DEQ(tq);

    node->condition = parse_expr(tq, 0);

    if (!expect(tq, CLOSE_PAREN)) {
        parser_error(")", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, SEMICOLON)) {
        parser_error(")", CUR(tq));
    }
    DEQ(tq);

    return node;
}

for_node* parse_for(token_queue* tq) {
    if (!expect(tq, KEYW_FOR)) {
        parser_error("for", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, OPEN_PAREN)) {
        parser_error("(", CUR(tq));
    }
    DEQ(tq);

    for_node* node = MALLOC(for_node);
    node->init = parse_for_init(tq);
    node->condition = parse_optional_expr(tq, SEMICOLON);
    node->final_expr = parse_optional_expr(tq, CLOSE_PAREN);
    node->body = parse_statement(tq);

    return node;
}

return_node *parse_return(token_queue *tq)
{
    if (!expect(tq, KEYW_RETURN))
    {
        parser_error("return", CUR(tq));
    }
    DEQ(tq);

    return_node *node = MALLOC(return_node);

    node->expr = parse_expr(tq, 0);

    if (!expect(tq, SEMICOLON))
    {
        parser_error(";", CUR(tq));
    }
    DEQ(tq);

    return node;
}

if_node* parse_if(token_queue* tq) {

    if_node* node = MALLOC(if_node);

    if (!expect(tq, KEYW_IF)) {
        parser_error("if", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, OPEN_PAREN)) {
        parser_error("(", CUR(tq));
    }
    DEQ(tq);
    
    node->condition = parse_expr(tq, 0);

    if (!expect(tq, CLOSE_PAREN)) {
        parser_error(")", CUR(tq));
    }
    DEQ(tq);

    node->then_stmt = parse_statement(tq);

    if (CUR(tq)->id == KEYW_ELSE) {
        // we have an else statement, lets parse it...
        DEQ(tq);
        node->else_stmt = parse_statement(tq);
    }
    
    return node;
}

statement_node *parse_statement(token_queue *tq)
{
    statement_node *node = MALLOC(statement_node);

    if (CUR(tq)->id == KEYW_RETURN)
    {
        node->type = STMT_RET;
        node->stmt.ret = parse_return(tq);
    }
    else if (CUR(tq)->id == SEMICOLON)
    {
        node->type = STMT_NULL;
        DEQ(tq);
    }
    else if (CUR(tq)->id == KEYW_IF)
    {
        node->type = STMT_IF;
        node->stmt.if_stmt = parse_if(tq);
    } else if (CUR(tq)->id == OPEN_BRACE) {
        node->type = STMT_COMPOUND;
        node->stmt.compound = parse_compound_statement(tq);
    } else if (CUR(tq)->id == KEYW_BREAK) {
        node->type = STMT_BREAK;
        node->stmt.break_node = parse_break(tq);
    } else if (CUR(tq)->id == KEYW_CONTINUE) {
        node->type = STMT_CONTINUE;
        node->stmt.continue_node = parse_continue(tq);
    } else if (CUR(tq)->id == KEYW_WHILE) {
        node->type = STMT_WHILE;
        node->stmt.while_node = parse_while(tq);
    } else if (CUR(tq)->id == KEYW_DO) {
        node->type = STMT_DO_WHILE;
        node->stmt.do_while_node = parse_do_while(tq);
    } else if (CUR(tq)->id == KEYW_FOR) {
        node->type = STMT_FOR;
        node->stmt.for_node = parse_for(tq);
    } 
    else
    {
        node->type = STMT_EXPR;
        node->stmt.expr = parse_expr(tq, 0);
        if (!expect(tq, SEMICOLON))
        {
            parser_error(";", CUR(tq));
        }
        DEQ(tq); // eat ;
    }

    return node;
}

declaration_node *parse_declaration(token_queue *tq)
{
    declaration_node *node = MALLOC(declaration_node);

    if (!expect(tq, KEYW_INT))
    {
        parser_error("int", CUR(tq));
    }
    DEQ(tq);

    if (!expect(tq, IDENTIFIER))
    {
        parser_error("an identifier", CUR(tq));
    }
    node->identifier = CUR(tq)->val.s;
    DEQ(tq);

    // check for optional initialiazer...
    if (CUR(tq)->id == EQUAL)
    {
        // we have an initializer expr, parse it:
        DEQ(tq);
        node->init = parse_expr(tq, 0);
    }

    if (!expect(tq, SEMICOLON))
    {
        parser_error(";", CUR(tq));
    }
    DEQ(tq);

    return node;
}

block_item_node *parse_block_item(token_queue *tq)
{
    block_item_node *block_item = MALLOC(block_item_node);

    if (CUR(tq)->id == KEYW_INT)
    {
        // declaration
        block_item->type = BLOCK_DECLARE;
        block_item->item.declare = parse_declaration(tq);
    }
    else
    {
        // statement
        block_item->type = BLOCK_STMT;
        block_item->item.stmt = parse_statement(tq);
    }

    return block_item;
}

block_node* parse_block(token_queue *tq) {
    block_node* block = MALLOC(block_node);

    if (!expect(tq, OPEN_BRACE))
    {
        parser_error("{", CUR(tq));
    }
    DEQ(tq);

    // collect the block items
    block->items = list_init();

    while (CUR(tq)->id != CLOSE_BRACE)
    {
        // build block body
        block_item_node *b = parse_block_item(tq);
        list_append(block->items, (void *)b);
    }

    if (!expect(tq, CLOSE_BRACE))
    {
        parser_error("}", CUR(tq));
    }
    DEQ(tq);

    return block;
}

compound_node* parse_compound_statement(token_queue* tq) {
    compound_node* node = MALLOC(compound_node);

    node->block = parse_block(tq);

    return node;
}

function_node *parse_function(token_queue *tq)
{
    if (!expect(tq, KEYW_INT))
    {
        parser_error("int", CUR(tq));
    }
    DEQ(tq);

    function_node *node = MALLOC(function_node);

    if (!expect(tq, IDENTIFIER))
    {
        parser_error("a function identifier", CUR(tq));
    }
    node->identifier = CUR(tq)->val.s;
    DEQ(tq);

    if (!expect(tq, OPEN_PAREN))
    {
        parser_error("(", CUR(tq));
    }
    DEQ(tq);

    // "void" is optional
    if (expect(tq, KEYW_VOID))
    {
        DEQ(tq);
    }

    if (!expect(tq, CLOSE_PAREN))
    {
        parser_error(")", CUR(tq));
    }
    DEQ(tq);

    node->body = parse_block(tq);

    return node;
}

program_node *parse_program(token_queue *tq)
{
    program_node *node = MALLOC(program_node);

    node->function = parse_function(tq);

    return node;
}