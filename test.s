.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $1, -4(%rbp)
	movl $2, -8(%rbp)
	cmpl $1, -8(%rbp)
	movl $0, -12(%rbp)
	setg -12(%rbp)
	cmpl $0, -12(%rbp)
	je .L_end.0
	movl $3, -8(%rbp)
	movl $4, -16(%rbp)
.L_end.0:
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
