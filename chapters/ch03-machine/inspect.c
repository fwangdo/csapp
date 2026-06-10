#include <stdio.h>

long sum(long *values, int n) {
    long result = 0;
    for (int i = 0; i < n; i++) {
        result += values[i];
    }
    return result;
}

void swap(long *a, long *b) {
    long tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void) {
    long values[] = {1, 2, 3, 4};
    long a = 10;
    long b = 20;

    swap(&a, &b);
    printf("sum=%ld a=%ld b=%ld\n", sum(values, 4), a, b);
    return 0;
}
