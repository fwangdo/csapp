/* Native-only test harness. Only trans.c receives coverage instrumentation. */
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"

extern trans_func_t func_list[MAX_TRANS_FUNCS];
extern int func_counter;
extern void registerFunctions(void);

static int *matrix_a, *matrix_b;
static size_t matrix_bytes;
static uint64_t b_base, accesses;
static FILE *trace;
static int recording;
static unsigned char valid[32];
static uint64_t tags[32];
static unsigned int hits, misses, evictions;

static void record_access(const void *pointer, unsigned size, char op)
{
    if (!recording)
        return;
    uintptr_t address = (uintptr_t)pointer;
    uintptr_t a = (uintptr_t)matrix_a, b = (uintptr_t)matrix_b;
    uint64_t mapped;
    size_t offset;
    if (address >= a && address - a < matrix_bytes) {
        offset = address - a;
        mapped = offset;
    } else if (address >= b && address - b < matrix_bytes) {
        offset = address - b;
        mapped = b_base + offset;
    } else {
        return; /* Stack/global bookkeeping is outside this array-only model. */
    }
    if (size > matrix_bytes - offset) {
        fprintf(stderr, "Array access crosses its allocation boundary\n");
        exit(2);
    }
    if (trace && fprintf(trace, " %c %" PRIx64 ",%u\n", op, mapped, size) < 0) {
        perror("write trace");
        exit(2);
    }
    unsigned int set = (mapped >> 5) & 31;
    uint64_t tag = mapped >> 10;
    if (valid[set] && tags[set] == tag) {
        hits++;
    } else {
        misses++;
        evictions += valid[set];
        valid[set] = 1;
        tags[set] = tag;
    }
    accesses++;
}

/* Clang SanitizerCoverage callbacks, compiled without coverage themselves. */
void __sanitizer_cov_trace_pc(void) {}
#define CALLBACKS(size, type) \
    void __sanitizer_cov_load##size(type *p) { record_access(p, size, 'L'); } \
    void __sanitizer_cov_store##size(type *p) { record_access(p, size, 'S'); }
CALLBACKS(1, uint8_t)
CALLBACKS(2, uint16_t)
CALLBACKS(4, uint32_t)
CALLBACKS(8, uint64_t)
CALLBACKS(16, __int128)

static uint32_t next_random(uint32_t *state)
{
    *state ^= *state << 13;
    *state ^= *state >> 17;
    *state ^= *state << 5;
    return *state;
}

/* registerFunctions() must be called by the caller before running a case. */
int native_trans_run(int M, int N, int fn, uint32_t seed,
                     const char *trace_path, uint64_t base)
{
    if (sizeof(int) != 4 || M < 1 || N < 1 || M > 256 || N > 256 || !seed)
        return 2;
    if (fn < 0 || fn >= func_counter) {
        fprintf(stderr, "Function %d is not registered (count=%d)\n", fn, func_counter);
        return 2;
    }
    b_base = base;
    accesses = hits = misses = evictions = 0;
    memset(valid, 0, sizeof(valid));
    trace = NULL;
    size_t count = (size_t)M * N;
    matrix_bytes = count * sizeof(int);
    matrix_a = malloc(matrix_bytes);
    matrix_b = malloc(matrix_bytes);
    int *original = malloc(matrix_bytes);
    if (!matrix_a || !matrix_b || !original) {
        fprintf(stderr, "Allocation failed\n");
        free(original);
        free(matrix_b);
        free(matrix_a);
        return 2;
    }
    for (size_t i = 0; i < count; i++) {
        original[i] = matrix_a[i] = (int)(next_random(&seed) & INT_MAX);
        matrix_b[i] = -1; /* Input values are nonnegative: unwritten cells fail. */
    }
    if (trace_path)
        trace = fopen(trace_path, "w");
    if (trace_path && !trace) {
        perror("open trace");
        free(original);
        free(matrix_b);
        free(matrix_a);
        return 2;
    }
    printf("Function %d: %s\n", fn, func_list[fn].description);
    recording = 1;
    func_list[fn].func_ptr(M, N, (int (*)[M])matrix_a, (int (*)[N])matrix_b);
    recording = 0;
    if (trace && fclose(trace)) {
        perror("close trace");
        free(original);
        free(matrix_b);
        free(matrix_a);
        return 2;
    }
    int correct = 1;
    if (memcmp(matrix_a, original, matrix_bytes)) {
        fprintf(stderr, "Input matrix A was modified\n");
        correct = 0;
    }
    for (int i = 0; i < N && correct; i++) {
        for (int j = 0; j < M; j++) {
            if (matrix_b[(size_t)j * N + i] != original[(size_t)i * M + j]) {
                fprintf(stderr, "Wrong B[%d][%d]: expected %d, got %d\n", j, i,
                        original[(size_t)i * M + j], matrix_b[(size_t)j * N + i]);
                correct = 0;
                break;
            }
        }
    }
    if (correct && !accesses) {
        fprintf(stderr, "No array accesses recorded: instrumentation unavailable\n");
        correct = 0;
    }
    func_list[fn].correct = correct;
    func_list[fn].num_hits = hits;
    func_list[fn].num_misses = misses;
    func_list[fn].num_evictions = evictions;
    printf("NATIVE_TRANS_RESULT correct=%d accesses=%" PRIu64 "\n", correct, accesses);
    free(original);
    free(matrix_b);
    free(matrix_a);
    return correct ? 0 : 1;
}

#ifndef NATIVE_TRANS_LIBRARY
int main(int argc, char **argv)
{
    if (argc != 7) {
        fprintf(stderr, "Usage: native-trans M N function seed trace b-base\n");
        return 2;
    }
    registerFunctions();
    return native_trans_run(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]),
                            (uint32_t)strtoul(argv[4], NULL, 0), argv[5],
                            strtoull(argv[6], NULL, 0));
}
#endif
