int main() {
    int a = 1;
    int b;
    if (a > (5+5 % 10))
        b = a = a++;
    else
        b = 4;
    return a ? b & 1 ? 1 : 2 : 3;
}