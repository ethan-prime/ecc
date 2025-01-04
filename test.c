int add(int x, int y) {
    return x + y;
}

int main(void) {
    int z = add(2, 4);
    if (z == 6) {
        return 1;
    }
    return 0;
}