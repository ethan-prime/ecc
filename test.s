.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	movl $1, -4(%rbp)
	movl -4(%rbp), %r11d
	imull $2, %r11d
	movl %r11d, -4(%rbp)
	movl $3, -8(%rbp)
	movl -8(%rbp), %r11d
	imull $4, %r11d
	movl %r11d, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -12(%rbp)
	movl $5, %eax
	cdq
	movl $4, %r10d
	idiv %r10d
	movl %eax, -16(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -16(%rbp), %r10d
	addl %r10d, -20(%rbp)
	movl -20(%rbp), %eax
	cdq
	movl $2, %r10d
	idiv %r10d
	movl %edx, -24(%rbp)
	movl -24(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
