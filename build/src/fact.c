int fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

void main() {
    int i;
    
    i = 0;
    while (i <= 12) {
        output(fact(i, 1));
        i = i + 1;
    }
}