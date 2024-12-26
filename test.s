.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $28, %rsp
	movl $1, -4(%rbp)
	movl $0, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	subl $1, -12(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -4(%rbp)
	cmpl $0, -16(%rbp)
	je .L_sc.0
	movl -8(%rbp), %r10d
	movl %r10d, -20(%rbp)
	addl $1, -20(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -8(%rbp)
	cmpl $0, -8(%rbp)
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
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
