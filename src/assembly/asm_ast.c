#include "asm_ast.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

// STATEMENTS:
list(asm_instruction_node) * return_to_asm(return_node *ret)
{
    list *l = list_init();

    asm_instruction_node *mov = MALLOC(asm_instruction_node);
    asm_instruction_node *ret_ = MALLOC(asm_instruction_node);

    operand_node src, dest;
    src.type = IMM;
    dest.type = REG;

    src.operand.imm.i = ret->expr->constant->val;
    dest.operand.reg.reg_name = "%eax";

    asm_move_node inst_mov;
    inst_mov.src = src;
    inst_mov.dest = dest;

    mov->type = INST_MOV;
    mov->instruction.mov = inst_mov;

    asm_return_node inst_ret;

    ret_->type = INST_RET;
    ret_->instruction.ret = inst_ret;

    list_append(l, (void *)mov);
    list_append(l, (void *)ret_);

    return l;
}

list(asm_instruction_node *) * statement_to_asm(statement_node *stmt)
{
    // we'll have to add more here later, rn we only have return stmts
    return return_to_asm(stmt->ret);
}

asm_function_node *function_to_asm(function_node *function)
{
    asm_function_node *node = MALLOC(asm_function_node);
    node->identifier = function->identifier;
    node->instructions = statement_to_asm(function->body);

    return node;
}

asm_program_node *program_to_asm(program_node *program)
{
    asm_program_node *prgm = MALLOC(asm_program_node);
    prgm->function = function_to_asm(program->function);

    return prgm;
}