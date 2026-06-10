#include <pthread.h>
#include <stdio.h>

enum { NTHREADS = 4, NINCR = 100000 };

static long counter;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg) {
    int use_lock = *(int *)arg;
    for (int i = 0; i < NINCR; i++) {
        if (use_lock) {
            pthread_mutex_lock(&lock);
        }
        counter++;
        if (use_lock) {
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

static long run(int use_lock) {
    pthread_t threads[NTHREADS];
    counter = 0;

    for (int i = 0; i < NTHREADS; i++) {
        pthread_create(&threads[i], NULL, worker, &use_lock);
    }
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return counter;
}

int main(void) {
    printf("without lock: %ld\n", run(0));
    printf("with lock:    %ld\n", run(1));
    return 0;
}
