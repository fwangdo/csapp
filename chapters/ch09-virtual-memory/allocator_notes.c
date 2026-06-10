#include <assert.h>
#include <stddef.h>
#include <stdio.h>

enum { ALIGNMENT = 16 };

static size_t align(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(size_t)(ALIGNMENT - 1);
}

static size_t pack(size_t size, int allocated) {
    return size | (allocated ? 1u : 0u);
}

static size_t block_size(size_t header) {
    return header & ~(size_t)0xf;
}

static int is_allocated(size_t header) {
    return (int)(header & 1u);
}

int main(void) {
    size_t header = pack(align(13), 1);
    assert(block_size(header) == 16);
    assert(is_allocated(header));
    printf("header=%zu size=%zu allocated=%d\n", header, block_size(header), is_allocated(header));
    return 0;
}
