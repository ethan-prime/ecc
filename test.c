int add(int x, int y) {
    return x + y;
}

int main(void) {
    int add(int a, int b);
    int z = add(9, 10);
    if (z == 21) {
        return 1;
    } else {
        return 0;
    }
}