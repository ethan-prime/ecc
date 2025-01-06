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