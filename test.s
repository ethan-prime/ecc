.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $28, %rsp
	movl $1, -4(%rbp)
	movl $7, -8(%rbp)
	cmpl $1, -8(%rbp)
	movl $0, -12(%rbp)
	setg -12(%rbp)
	cmpl $0, -12(%rbp)
	je .L_sc.0
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	andl $2, -16(%rbp)
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
	je .L_end.1
	movl $3, -8(%rbp)
	movl $4, -28(%rbp)
.L_end.1:
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
