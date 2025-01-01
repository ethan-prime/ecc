# ecc
Ethan's C Compiler, written in C!

## Status
```
features:
- if statements
- return statements
- local variables
- arbitrary binary expressions
- compound statements
- while, do while, and for loops
```

```
calculates the nth fibonacci number.
```
```c
int main(void) {
    int a = 1;
    int b = 0;
    for (int i = 2; i <= 9; i++) {
        int tmp = a;
        a = a + b;
        b = tmp;
    }
    return a;
}
```

```assembly
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $28, %rsp
	movl $1, -4(%rbp)
	movl $0, -8(%rbp)
	movl $2, -12(%rbp)
.L_start_loop.0:
	cmpl $9, -12(%rbp)
	movl $0, -16(%rbp)
	setle -16(%rbp)
	cmpl $0, -16(%rbp)
	je .L_break_loop.0
	movl -4(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -24(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -24(%rbp)
	movl -24(%rbp), %r10d
	movl %r10d, -4(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -8(%rbp)
.L_continue_loop.0:
	movl -12(%rbp), %r10d
	movl %r10d, -20(%rbp)
	addl $1, -20(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -12(%rbp)
	jmp .L_start_loop.0
.L_break_loop.0:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
```

```c
int main(void) {
    int n = 10;
    int m = 0;
    while (n-- || (m % 2)) {
        m += n;
        if (m > 10)
            break;
    }
    return m;       
}
```

```assembly
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $10, -4(%rbp)
	movl $0, -8(%rbp)
.L_continue_loop.0:
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	subl $1, -12(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -4(%rbp)
	cmpl $0, -16(%rbp)
	jne .L_sc.0
	movl -8(%rbp), %eax
	cdq
	movl $2, %r10d
	idiv %r10d
	movl %edx, -20(%rbp)
	cmpl $0, -20(%rbp)
	jne .L_sc.0
	movl $0, -24(%rbp)
	jmp .L_end.0
.L_sc.0:
	movl $1, -24(%rbp)
.L_end.0:
	cmpl $0, -24(%rbp)
	je .L_break_loop.0
	movl -8(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -4(%rbp), %r10d
	addl %r10d, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -8(%rbp)
	cmpl $10, -8(%rbp)
	movl $0, -32(%rbp)
	setg -32(%rbp)
	cmpl $0, -32(%rbp)
	je .L_end.1
	jmp .L_break_loop.0
.L_end.1:
	jmp .L_continue_loop.0
.L_break_loop.0:
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
```