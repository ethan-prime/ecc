#include "../config.h"
#if CODEGEN_ENABLE
#include "codegen.h"
#include <stdio.h>
#include "../utils/symboltable.h"

void emit(FILE *file, char *t)
{
    fprintf(file, "\t%s\n", t);
}

void emit_suffix(FILE* file, op_size size) {
    if (size == OP_1_BYTE) {
        fprintf(file, "b");        
    } else if (size == OP_4_BYTES) {
        fprintf(file, "l");
    }
}

void emit_register(FILE *file, asm_register_t reg, op_size size)
{
    const char *registers[][3] = 
    {
        {"%al", "%eax", "%rax"},   // AX
        {"%dl", "%edx", "%rdx"},   // DX
        {"%cl", "%ecx", "%rcx"},   // CX
        {"%dil", "%edi", "%rdi"},  // DI
        {"%sil", "%esi", "%rsi"},  // SI
        {"%r8b", "%r8d", "%r8"},   // R8
        {"%r9b", "%r9d", "%r9"},   // R9
        {"%r10b", "%r10d", "%r10"}, // R10
        {"%r11b", "%r11d", "%r11"} // R11
    };

    int size_index = (size == OP_1_BYTE) ? 0 :
                     (size == OP_4_BYTES) ? 1 : 2;

    fprintf(file, "%s", registers[reg][size_index]);
}

void emit_operand(FILE *file, operand_node *op, op_size size)
{
    switch (op->type)
    {
    case IMM:
        fprintf(file, "$%d", op->operand.imm->i);
        break;
    case REG:
        emit_register(file, op->operand.reg->reg, size);
        break;
    case STACK:
        fprintf(file, "%d(%%rbp)", op->operand.stack->offset);
    default:
        break;
    }
}

void emit_mov_instr(FILE *file, asm_move_node *mov)
{
    fprintf(file, "\tmov");
    emit_suffix(file, mov->size);
    fprintf(file, " ");
    emit_operand(file, mov->src, mov->size);
    fprintf(file, ", ");
    emit_operand(file, mov->dest, mov->size);
    fprintf(file, "\n");
}

void emit_unary_op(FILE *file, asm_unary_op op)
{
    switch (op)
    {
    case ASM_NOT:
        fprintf(file, "notl");
        break;
    case ASM_NEG:
        fprintf(file, "negl");
        break;
    }
}

void emit_unary_instr(FILE *file, asm_unary_node *unary)
{
    fprintf(file, "\t");
    emit_unary_op(file, unary->op);
    fprintf(file, " ");
    emit_operand(file, unary->dest, OP_4_BYTES);
    fprintf(file, "\n");
}

void emit_binary_op(FILE *file, asm_binary_op op)
{
    switch (op)
    {
    case ASM_ADD:
        fprintf(file, "addl");
        break;
    case ASM_SUB:
        fprintf(file, "subl");
        break;
    case ASM_MULT:
        fprintf(file, "imull");
        break;
    case ASM_AND:
        fprintf(file, "andl");
        break;
    case ASM_OR:
        fprintf(file, "orl");
        break;
    case ASM_XOR:
        fprintf(file, "xor");
        break;
    case ASM_LEFT_SHIFT:
        fprintf(file, "shll");
        break;
    case ASM_RIGHT_SHIFT:
        fprintf(file, "shrl");
        break;
    default:
        break;
    }
}

void emit_binary_instr(FILE *file, asm_binary_node *binary)
{
    fprintf(file, "\t");
    emit_binary_op(file, binary->op);
    fprintf(file, " ");
    emit_operand(file, binary->src, OP_4_BYTES);
    fprintf(file, ", ");
    emit_operand(file, binary->dest, OP_4_BYTES);
    fprintf(file, "\n");
}

void emit_idiv_instr(FILE *file, asm_idiv_node *idiv)
{
    fprintf(file, "\tidiv ");
    emit_operand(file, idiv->divisor, OP_4_BYTES);
    fprintf(file, "\n");
}

void emit_return_instr(FILE *file)
{
    emit(file, "movq %rbp, %rsp");
    emit(file, "popq %rbp");
    emit(file, "ret");
}

void emit_cdq_instr(FILE *file)
{
    emit(file, "cdq");
}

void emit_stackalloc_instr(FILE *file, asm_stackalloc_node *stackalloc)
{
    fprintf(file, "\tsubq $%d, %%rsp\n", stackalloc->n_bytes);
}

void emit_stackdealloc_instr(FILE* file, asm_stackdealloc_node* stackdealloc)
{
    fprintf(file, "\taddq $%d, %%rsp\n", stackdealloc->n_bytes);
}

void emit_cmp_instr(FILE* file, asm_cmp_node* cmp) {
    fprintf(file, "\tcmpl ");
    emit_operand(file, cmp->src1, OP_4_BYTES);
    fprintf(file, ", ");
    emit_operand(file, cmp->src2, OP_4_BYTES);
    fprintf(file, "\n");
}

void emit_jmp_instr(FILE* file, asm_jmp_node* jmp) {
    fprintf(file, "\tjmp .L%s\n", jmp->identifier);
}

void emit_cond_code(FILE* file, cond_code type) {
    if (type == G) {
        fprintf(file, "g");
    } else if (type == GE) {
        fprintf(file, "ge");
    } else if (type == L) {
        fprintf(file, "l");
    } else if (type == LE) {
        fprintf(file, "le");
    } else if (type == E) {
        fprintf(file, "e");
    } else if (type == NE) {
        fprintf(file, "ne");
    }
}

void emit_jmpcc_instr(FILE* file, asm_jmpcc_node* jmpcc) {
    fprintf(file, "\tj");
    emit_cond_code(file, jmpcc->type);
    fprintf(file, " .L%s\n", jmpcc->identifier);
}

void emit_setcc_instr(FILE* file, asm_setcc_node* setcc) {
    fprintf(file, "\tset");
    emit_cond_code(file, setcc->type);
    fprintf(file, " ");
    emit_operand(file, setcc->dest, OP_1_BYTE);
    fprintf(file, "\n");
}

void emit_label_instr(FILE* file, asm_label_node* label) {
    fprintf(file, ".L%s:\n", label->identifier);
}

void emit_push_instr(FILE* file, asm_push_node* push) {
    fprintf(file, "\tpushq ");
    emit_operand(file, push->op, OP_8_BYTES);
    fprintf(file, "\n");
}

void emit_call_instr(FILE* file, asm_call_node* call, symboltable* symbols) {
    fprintf(file, "\tcall %s", call->identifier);
    if (!(symboltable_get(symbols, call->identifier)->defined)) {
        // if the function is undefined, we need to reference the PLT.
        fprintf(file, "@PLT");
    }
    fprintf(file, "\n");
}

void emit_instruction(FILE *file, asm_instruction_node *instr, symboltable* symbols)
{
    switch (instr->type)
    {
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
    case INSTR_STACKDEALLOC:
        emit_stackdealloc_instr(file, instr->instruction.stackdealloc);
        break;
    case INSTR_BINARY:
        emit_binary_instr(file, instr->instruction.binary);
        break;
    case INSTR_IDIV:
        emit_idiv_instr(file, instr->instruction.idiv);
        break;
    case INSTR_CDQ:
        emit_cdq_instr(file);
        break;
    case INSTR_CMP:
        emit_cmp_instr(file, instr->instruction.cmp);
        break;
    case INSTR_JMP:
        emit_jmp_instr(file, instr->instruction.jmp);
        break;
    case INSTR_JMPCC:
        emit_jmpcc_instr(file, instr->instruction.jmpcc);
        break;
    case INSTR_SETCC:
        emit_setcc_instr(file, instr->instruction.setcc);
        break;
    case INSTR_LABEL:
        emit_label_instr(file, instr->instruction.label);
        break;
    case INSTR_PUSH:
        emit_push_instr(file, instr->instruction.push);
        break;
    case INSTR_CALL:
        emit_call_instr(file, instr->instruction.call, symbols);
    }
}

void emit_function(FILE *file, asm_function_node *function, symboltable* symbols)
{
    fprintf(file, ".globl %s\n", function->identifier);
    fprintf(file, "%s:\n", function->identifier);
    emit(file, "pushq %rbp");
    emit(file, "movq %rsp, %rbp");
    for (int i = 0; i < function->instructions->len; i++)
    {
        asm_instruction_node *instr = (asm_instruction_node *)list_get(function->instructions, i);
        emit_instruction(file, instr, symbols);
    }
}

void emit_program(FILE *file, asm_program_node *program, symboltable* symbols)
{
    for (int i = 0; i < program->functions->len; i++) {
        asm_function_node* func = (asm_function_node*)list_get(program->functions, i);
        emit_function(file, func, symbols);
        fprintf(file, "\n");
    }
    fprintf(file, ".section .note.GNU-stack,\"\",@progbits\n");
}
#endif
