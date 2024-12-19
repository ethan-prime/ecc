#include "codegen.h"
#include <stdio.h>

void operand_emit(FILE* file, operand_node* operand) {
    if (operand->type == IMM) {
        fprintf(file, "$%d", operand->operand.imm.i);
    } else if (operand->type == REG) {
        fprintf(file, "%s", operand->operand.reg.reg_name);
    }
}

void mov_emit(FILE* file, asm_move_node* mov) {
    fprintf(file, "\tmovl ");
    operand_emit(file, &mov->src);
    fprintf(file, ", ");
    operand_emit(file, &mov->dest);
    fprintf(file, "\n");
}

void ret_emit(FILE* file) {
    fprintf(file, "\tret\n");
}

void instruction_emit(FILE* file, asm_instruction_node* instr) {
    switch (instr->type) {
        case INST_MOV:
            mov_emit(file, &instr->instruction.mov); break;
        case INST_RET:
            ret_emit(file); break;
    }
}

void instructions_emit(FILE* file, list(asm_instruction_node*)* instrs) {
    for(int i = 0; i < instrs->len; i++) {
        asm_instruction_node* instr = (asm_instruction_node *)list_get(instrs, i);
        instruction_emit(file, instr);
    }
}

void function_emit(FILE* file, asm_function_node* function) {
    fprintf(file, ".globl %s\n", function->identifier);
    fprintf(file, "%s:\n", function->identifier);
    instructions_emit(file, function->instructions);
}

void program_emit(FILE* file, asm_program_node* program) {
    function_emit(file, program->function);
    fprintf(file, ".section .note.GNU-stack,\"\",@progbits\n");
}