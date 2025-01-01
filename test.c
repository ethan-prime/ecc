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