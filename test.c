int main() {
    int x = 1;
    {
        int x = 7;
        if (x > 1 && !(x & 2)) {
            x = 3;
            int x = 4;
        }
        return x;
    }
    return x;
}