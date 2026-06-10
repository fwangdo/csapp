#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { COUNT = 1 << 24 };

static double elapsed(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(void) {
    int *data = calloc(COUNT, sizeof(*data));
    if (data == NULL) {
        return 1;
    }

    for (int stride = 1; stride <= 1024; stride *= 2) {
        volatile long sum = 0;
        clock_t start = clock();
        for (int i = 0; i < COUNT; i += stride) {
            sum += data[i];
        }
        clock_t end = clock();
        printf("stride=%4d seconds=%.6f sum=%ld\n", stride, elapsed(start, end), sum);
    }

    free(data);
    return 0;
}
