#include <limits.h>
#include <stdio.h>

int bit_xor(int x, int y) {
    /* TODO: implement x ^ y using only ~ and &. */
    (void)x;
    (void)y;
    return 0;
}

int fits_in_short(int x) {
    /* TODO: return 1 if x can be represented as a signed 16-bit value. */
    (void)x;
    return 0;
}

int saturating_add(int x, int y) {
    /* TODO: return x + y, saturated to INT_MAX or INT_MIN on overflow. */
    (void)x;
    (void)y;
    return 0;
}

int main(void) {
    printf("bit_xor(4, 5): got=%d expected=1\n", bit_xor(4, 5));
    printf("fits_in_short(32767): got=%d expected=1\n", fits_in_short(32767));
    printf("fits_in_short(32768): got=%d expected=0\n", fits_in_short(32768));
    printf("saturating_add(INT_MAX, 1): got=%d expected=%d\n",
           saturating_add(INT_MAX, 1), INT_MAX);
    printf("saturating_add(INT_MIN, -1): got=%d expected=%d\n",
           saturating_add(INT_MIN, -1), INT_MIN);
    return 0;
}
