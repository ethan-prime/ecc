#include "codegen.h"
#include <stdio.h>

void emit(FILE* file, char* t) {
    fprintf(file, "\t%s\n", t);
}

void emit_register(FILE* file, asm_register_t reg) {
    switch (reg) {
        case AX:
            fprintf(file, "%%eax");
            break;
        case R10:
            fprintf(file, "%%r10d");
            break;
    }
}

void emit_operand(FILE* file, operand_node* op) {
    switch (op->type) {
        case IMM:
            fprintf(file, "$%d", op->operand.imm->i);
            break;
        case REG:
            emit_register(file, op->operand.reg->reg);
            break;
        case STACK:
            fprintf(file, "-%d(%%rbp)", op->operand.stack->offset);
    }
}

void emit_mov_instr(FILE* file, asm_move_node* mov) {
    fprintf(file, "\tmovl ");
    emit_operand(file, mov->src);
    fprintf(file, ", ");
    emit_operand(file, mov->dest);
    fprintf(file, "\n");
}

void emit_unary_op(FILE* file, asm_unary_op op) {
    switch (op) {
        case ASM_NOT:
            fprintf(file, "notl");
            break;
        case ASM_NEG:
            fprintf(file, "negl");
            break;
    }
}

void emit_unary_instr(FILE* file, asm_unary_node* unary) {
    fprintf(file, "\t");
    emit_unary_op(file, unary->op);
    fprintf(file, " ");
    emit_operand(file, unary->dest);
    fprintf(file, "\n");
}

void emit_return_instr(FILE* file) {
    emit(file, "movq %rbp, %rsp");
    emit(file, "popq %rbp");
    emit(file, "ret");
}

void emit_stackalloc_instr(FILE* file, asm_stackalloc_node* stackalloc) {
    fprintf(file, "\tsubq $%d, %%rsp\n", stackalloc->n_bytes);
}

void emit_instruction(FILE* file, asm_instruction_node* instr) {
    switch (instr->type) {
        case INSTR_MOV:
            emit_mov_instr(file, instr->instruction.mov);
            break;
        case INSTR_UNARY:
            emit_unary_instr(file, instr->instruction.unary);
            break;
        case INSTR_RET:
            emit_return_instr(file);
            break;
        case INSTR_STACKALLOC:
            emit_stackalloc_instr(file, instr->instruction.stackalloc);
            break;
    }
}

void emit_function(FILE* file, asm_function_node* function) {
    fprintf(file, ".globl %s\n", function->identifier);
    fprintf(file, "%s:\n", function->identifier);
    emit(file, "pushq %rbp");
    emit(file, "movq %rsp, %rbp");
    for (int i = 0; i < function->instructions->len; i++) {
        asm_instruction_node* instr = (asm_instruction_node*)list_get(function->instructions, i);
        emit_instruction(file, instr);
    }
}

void emit_program(FILE* file, asm_program_node* program) {
    emit_function(file, program->function);
    fprintf(file, "\n");
    fprintf(file, ".section .note.GNU-stack,\"\",@progbits\n");
}