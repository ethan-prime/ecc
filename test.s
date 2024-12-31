.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movl $10, -4(%rbp)
	movl $0, -8(%rbp)
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
