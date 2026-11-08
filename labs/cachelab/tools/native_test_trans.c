/* Native CLI for the Clang-instrumented transpose harness. */
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cachelab.h"

extern trans_func_t func_list[MAX_TRANS_FUNCS];
extern int func_counter;
extern void registerFunctions(void);
extern int native_trans_run(int M, int N, int fn, uint32_t seed,
                            const char *trace_path, uint64_t base);

static int number(const char *text)
{
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno || end == text || *end || value < 0 || value > INT_MAX)
        return -1;
    return (int)value;
}

static void usage(const char *program)
{
    printf("Usage: %s -M <input columns> -N <input rows> [-F <function>]\n"
           "Dimensions: 1..256. Default: test all registered functions.\n"
           "Native array-access model: s=5 E=1 b=5; not official grading.\n",
           program);
}

/* A broken registered function must not prevent testing the remaining ones. */
static int run_function(int M, int N, int fn)
{
    struct {
        int status;
        unsigned int hits, misses, evictions;
    } result;
    int channel[2];
    if (pipe(channel)) {
        perror("pipe");
        return 2;
    }
    fflush(NULL);
    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        close(channel[0]);
        close(channel[1]);
        return 2;
    }
    if (child == 0) {
        close(channel[0]);
        alarm(15);
        result.status = native_trans_run(M, N, fn, 1, NULL, 262144);
        result.hits = func_list[fn].num_hits;
        result.misses = func_list[fn].num_misses;
        result.evictions = func_list[fn].num_evictions;
        if (write(channel[1], &result, sizeof(result)) != sizeof(result))
            result.status = 2;
        close(channel[1]);
        exit(result.status);
    }
    close(channel[1]);
    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited < 0 && errno == EINTR);
    ssize_t received;
    do {
        received = read(channel[0], &result, sizeof(result));
    } while (received < 0 && errno == EINTR);
    close(channel[0]);
    if (waited < 0) {
        perror("waitpid");
        return 2;
    }
    if (!WIFEXITED(status) || received != sizeof(result)) {
        fprintf(stderr, "Function %d crashed, timed out, or failed a sanitizer check\n", fn);
        return 1;
    }
    func_list[fn].correct = result.status == 0;
    func_list[fn].num_hits = result.hits;
    func_list[fn].num_misses = result.misses;
    func_list[fn].num_evictions = result.evictions;
    return result.status;
}

int main(int argc, char **argv)
{
    int M = 0, N = 0, selected = -1, option;
    while ((option = getopt(argc, argv, "M:N:F:h")) != -1) {
        switch (option) {
        case 'M': M = number(optarg); break;
        case 'N': N = number(optarg); break;
        case 'F':
            selected = number(optarg);
            if (selected < 0) {
                fprintf(stderr, "Invalid function number\n");
                return 2;
            }
            break;
        case 'h': usage(argv[0]); return 0;
        default: usage(argv[0]); return 2;
        }
    }
    if (optind != argc || M < 1 || N < 1 || M > 256 || N > 256) {
        usage(argv[0]);
        return 2;
    }
    registerFunctions();
    if (!func_counter || selected >= func_counter) {
        fprintf(stderr, "Function is not registered (count=%d)\n", func_counter);
        return 2;
    }
    alarm(120);
    puts("Native transpose test: s=5 E=1 b=5; A=0 B=0x40000\n"
         "Miss counts use a Clang -O0 array-only model, not official grading.");
    int submission = -1;
    for (int fn = 0; fn < func_counter; fn++) {
        if (selected >= 0 && fn != selected)
            continue;
        if (selected >= 0 || !strcmp(func_list[fn].description, "Transpose submission"))
            submission = fn;
        int status = run_function(M, N, fn);
        if (status == 2)
            return 2;
        printf("%s func %d (%s): hits:%u misses:%u evictions:%u\n\n",
               status == 0 ? "PASS" : "FAIL", fn, func_list[fn].description,
               func_list[fn].num_hits, func_list[fn].num_misses,
               func_list[fn].num_evictions);
    }
    if (submission < 0) {
        fprintf(stderr, "No function with description \"Transpose submission\" found\n");
        puts("TEST_TRANS_RESULTS=0:0");
        return 1;
    }
    trans_func_t *result = &func_list[submission];
    printf("TEST_TRANS_RESULTS=%d:%u\n", result->correct,
           result->correct ? result->num_misses : 0);
    return result->correct ? 0 : 1;
}
