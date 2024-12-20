#include "passes.h"
#include "asm_ast.h"
#include "../utils/stackmap.h"
#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

void replace_pseudo_stack(stackmap* sm, operand_node* pseudo_node) {
    if (pseudo_node->type != PSEUDO) {
        return;
    }
    char* iden = pseudo_node->operand.pseudo->reg_name;
    if (stackmap_getoffset(sm, iden) == -1) {
        // we gotta add it...
        stackmap_add(sm, iden, 4); // add 4 bytes.
    }

    pseudo_node->type = STACK; // switch this to a stack operand.
    asm_stack_node* stack = MALLOC(asm_stack_node);
    stack->offset = stackmap_getoffset(sm, iden);
    free(pseudo_node->operand.pseudo);
    pseudo_node->operand.stack = stack;
}

void replace_pseudo_function(asm_function_node* function) {
    stackmap* sm = stackmap_init();

    list(asm_instruction_node*)* instrs = function->instructions;
    asm_instruction_node* instr;

    // iterate through each instruction, replacing pseudo stores with stack stores...
    for (int i = 0; i < instrs->len; i++) {
        instr = (asm_instruction_node*)list_get(instrs, i);
        if (instr->type == INSTR_MOV) {
            if (instr->instruction.mov->src->type == PSEUDO) {
                replace_pseudo_stack(sm, instr->instruction.mov->src);
            }
            if (instr->instruction.mov->dest->type == PSEUDO) {
                replace_pseudo_stack(sm, instr->instruction.mov->dest);
            }
        } else if (instr->type == INSTR_UNARY) {
            if (instr->instruction.unary->dest->type == PSEUDO) {
                replace_pseudo_stack(sm, instr->instruction.unary->dest);
            }
        }
    }
}

void replace_pseudo_pass(asm_program_node* program) {
    replace_pseudo_function(program->function);
}
