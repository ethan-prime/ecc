# ecc
Ethan's C Compiler, written in C! (for Linux)
Following Nora Sandler's "Writing a C Compiler" book

## Dependencies
- gcc

## Status
Features:
- Types: int
- If Statements
- Return Statements
- Local Variables
- Unary Expressions (!, -, ~)
- Binary Expressions (arbitrarily complex)
- Compound Statements
- While, Do While, and For Loops
- Function Calls
- Library Functions
- Compiling to Object Files (-c)

## Looking to contribute?
Pull requests are welcome! Here is a list of things I think could use some re-factoring:
- Implement src/utils/identifiermap.[ch] using the src/utils/utils_hashmap.[ch] instead of src/utils/utils_list.[ch]

## Examples

```
printint.c - prints int to stdout:
```
```c
int putchar(int c);

int printinthelper(int n) {
    if (n < 10) {
        putchar(n + 48);
    } else {
        printinthelper(n / 10);
        putchar(n % 10 + 48);
    }
}

int printint(int n) {
    if (n < 0) {
        putchar(45);
        printinthelper(-n);
    } else {
        printinthelper(n);
    }
    putchar(10);
}
```

```
fib.c - calculates and prints the nth fibonacci number:
```
```c
int printint(int n);

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 2) + fibonacci(n - 1);
}

int main(void) {
    int fib = fibonacci(11);
    printint(fib);
    return 0;
}
```

```
./ecc fib.c printint.c
```

```
printint.s
```
```assembly
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
```

```
fib.s
```
```assembly
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
```