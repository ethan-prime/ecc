# ecc
Ethan's C Compiler, written in C!

## Status
```
It can compile arbitrary variable assignments and return expressions that consist of integers. See examples below!
All binary operators supported, including compounds such as +=, *=, &=, etc.
++ and -- supported (extremely annoying to implement, lol)
```

```c
int main() {
    int a = 2;
    int b;
    b = a = 3 >= a;
    return 2*b + 5;
}
```

```assembly
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $20, %rsp
	movl $2, -4(%rbp)
	movl $3, %r11d
	cmpl -4(%rbp), %r11d
	movl $0, -8(%rbp)
	setle -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl $2, -16(%rbp)
	movl -16(%rbp), %r11d
	imull -12(%rbp), %r11d
	movl %r11d, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -20(%rbp)
	addl $5, -20(%rbp)
	movl -20(%rbp), %eax
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
int main() {
    int a = 1;
    int b = 0;
    return ((a-- && ++b) || 0);
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
```