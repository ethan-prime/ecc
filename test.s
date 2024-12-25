.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $28, %rsp
	movl $5, %r11d
	cmpl $4, %r11d
	movl $0, -4(%rbp)
	setl -4(%rbp)
	cmpl $0, -4(%rbp)
	movl $0, -8(%rbp)
	sete -8(%rbp)
	cmpl $0, -8(%rbp)
	je .L_sc.0
	movl $5, -12(%rbp)
	addl $1, -12(%rbp)
	movl -12(%rbp), %eax
	cdq
	movl $2, %r10d
	idiv %r10d
	movl %edx, -16(%rbp)
	cmpl $0, -16(%rbp)
	movl $0, -20(%rbp)
	sete -20(%rbp)
	cmpl $0, -20(%rbp)
	je .L_sc.0
	movl $1, -24(%rbp)
	jmp .L_end.0
.L_sc.0:
	movl $0, -24(%rbp)
.L_end.0:
	cmpl $0, -24(%rbp)
	jne .L_sc.1
	movl $0, %r11d
	cmpl $0, %r11d
	jne .L_sc.1
	movl $0, -28(%rbp)
	jmp .L_end.1
.L_sc.1:
	movl $1, -28(%rbp)
.L_end.1:
	movl -28(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
