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