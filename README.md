# ecc
Ethan's C Compiler, written in C!

## Status
It can compile arbitrary return expressions that consist of integers. See an example below!

```c
int main() {
    return (1*2+(3*4)+5/4) % 2;
}
```