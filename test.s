.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $40, %rsp
	movl $1, -4(%rbp)
	movl $5, %eax
	cdq
	movl $10, %r10d
	idiv %r10d
	movl %edx, -8(%rbp)
	movl $5, -12(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -12(%rbp)
	movl -12(%rbp), %r10d
	cmpl %r10d, -4(%rbp)
	movl $0, -16(%rbp)
	setg -16(%rbp)
	cmpl $0, -16(%rbp)
	je .L_else.0
	movl -4(%rbp), %r10d
	movl %r10d, -20(%rbp)
	addl $1, -20(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -24(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -4(%rbp)
	movl -24(%rbp), %r10d
	movl %r10d, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -28(%rbp)
	jmp .L_end.0
.L_else.0:
	movl $4, -28(%rbp)
.L_end.0:
	cmpl $0, -4(%rbp)
	je .L_else.1
	movl -28(%rbp), %r10d
	movl %r10d, -32(%rbp)
	andl $1, -32(%rbp)
	cmpl $0, -32(%rbp)
	je .L_else.2
	movl $1, -36(%rbp)
	jmp .L_end.1
.L_else.2:
	movl $2, -36(%rbp)
.L_end.1:
	movl -36(%rbp), %r10d
	movl %r10d, -40(%rbp)
	jmp .L_end.2
.L_else.1:
	movl $3, -40(%rbp)
.L_end.2:
	movl -40(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
