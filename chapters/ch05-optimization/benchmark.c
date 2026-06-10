#include <stdio.h>
#include <time.h>

enum { N = 1000000 };

static long data[N];

long combine1(const long *a, int n) {
    long result = 0;
    for (int i = 0; i < n; i++) {
        result = result + a[i];
    }
    return result;
}

long combine2(const long *a, int n) {
    /* TODO: try loop unrolling or multiple accumulators here. */
    long result = 0;
    for (int i = 0; i < n; i++) {
        result += a[i];
    }
    return result;
}

static double elapsed(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(void) {
    for (int i = 0; i < N; i++) {
        data[i] = i & 7;
    }

    clock_t start = clock();
    long result = combine2(data, N);
    clock_t end = clock();

    printf("result=%ld seconds=%.6f\n", result, elapsed(start, end));
    return 0;
}
