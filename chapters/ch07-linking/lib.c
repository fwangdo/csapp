#include "lib.h"

static int bias = 1;

int add(int a, int b) {
    return a + b + bias;
}
