int main() {
    int a = 10;
    int b;
    if (a > 10)
        b = a = 1;
    else
        b = 4;
    return a ? b ? 1 : 2 : 3;
}