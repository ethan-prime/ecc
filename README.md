# ecc
Ethan's C Compiler, written in C!

## Status
It can compile arbitrary return expressions that consist of integers. See an example below!

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