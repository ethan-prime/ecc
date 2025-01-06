.globl printinthelper
printinthelper:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movl %edi, -4(%rbp)
	cmpl $10, -4(%rbp)
	movl $0, -8(%rbp)
	setl -8(%rbp)
	cmpl $0, -8(%rbp)
	je .L_else.0
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	addl $48, -12(%rbp)
	movl -12(%rbp), %edi
	call putchar@PLT
	movl %eax, -16(%rbp)
	jmp .L_end.1
.L_else.0:
	movl -4(%rbp), %eax
	cdq
	movl $10, %r10d
	idiv %r10d
	movl %eax, -20(%rbp)
	movl -20(%rbp), %edi
	call printinthelper
	movl %eax, -24(%rbp)
	movl -4(%rbp), %eax
	cdq
	movl $10, %r10d
	idiv %r10d
	movl %edx, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -32(%rbp)
	addl $48, -32(%rbp)
	movl -32(%rbp), %edi
	call putchar@PLT
	movl %eax, -36(%rbp)
.L_end.1:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.globl printint
printint:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl %edi, -4(%rbp)
	cmpl $0, -4(%rbp)
	movl $0, -8(%rbp)
	setl -8(%rbp)
	cmpl $0, -8(%rbp)
	je .L_else.1
	movl $45, %edi
	call putchar@PLT
	movl %eax, -12(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	negl -16(%rbp)
	movl -16(%rbp), %edi
	call printinthelper
	movl %eax, -20(%rbp)
	jmp .L_end.2
.L_else.1:
	movl -4(%rbp), %edi
	call printinthelper
	movl %eax, -24(%rbp)
.L_end.2:
	movl $10, %edi
	call putchar@PLT
	movl %eax, -28(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
