.globl fibonacci
fibonacci:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl %edi, -4(%rbp)
	cmpl $1, -4(%rbp)
	movl $0, -8(%rbp)
	setle -8(%rbp)
	cmpl $0, -8(%rbp)
	je .L_end.0
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
.L_end.0:
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	subl $2, -12(%rbp)
	movl -12(%rbp), %edi
	call fibonacci
	movl %eax, -16(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -20(%rbp)
	subl $1, -20(%rbp)
	movl -20(%rbp), %edi
	call fibonacci
	movl %eax, -24(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -24(%rbp), %r10d
	addl %r10d, -28(%rbp)
	movl -28(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $11, %edi
	call fibonacci
	movl %eax, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %edi
	call printint@PLT
	movl %eax, -12(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
