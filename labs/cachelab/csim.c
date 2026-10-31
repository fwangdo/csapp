#include "cachelab.h"
#include "inttypes.h"
#include "stdint.h"
#include "stdio.h"
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// stats
int hits;
int misses;
int evictions;

int s = -1;
int E = -1;
int b = -1;

char *trace_path;

// memory bits
int M = 64;
int t;

// structures
typedef struct line {
  bool valid; // need bull
  uint64_t tag;
  int used;
} line_t;

typedef struct set {
  int set_idx;
  line_t *lines;
} set_t;

set_t *cache;

// funcs. TO-DO.
bool initialize(int s, int E, int b) {
  size_t S = (size_t)1 << s;

  cache = malloc(S * sizeof(*cache));

  if (cache == NULL) {
    perror("malloc");
    return false;
  }

  // set.
  for (int i = 0; i < S; i++) {
    cache[i].set_idx = (int)i;

    // line..
    cache[i].lines = calloc(E, sizeof(*cache[i].lines));

    if (cache[i].lines == NULL) {
      perror("calloc");

      // 지금까지 성공한 할당만 해제
      for (size_t j = 0; j < i; j++) {
        free(cache[j].lines);
      }

      free(cache);
      cache = NULL;
      return false;
    }
  }

  return true;
};

void destory(int s, int E) {
  size_t S = (size_t)1 << s;
  for (int i = 0; i < S; i++) {
    free(cache[i].lines);
  }
  free(cache);
  cache = NULL;
};

uint64_t get_set_index(uint64_t address) {
  // t / s / b
  uint64_t b_bit = ((uint64_t)1 << b) - 1;
  uint64_t s_bit = ((uint64_t)1 << (s + b)) - 1;
  uint64_t only_s = s_bit - b_bit;
  return (address & only_s) >> b;
};
uint64_t get_tag_index(uint64_t address) { return address >> (s + b); };

void do_load(uint64_t set_idx, uint64_t tag_idx) {
  for (int i = 0; i < E; i++) {
  }
};
void do_store(uint64_t set_idx, uint64_t tag_idx) {

};
void do_modify(uint64_t set_idx, uint64_t tag_idx) {

};
void simulate(char sig, uint64_t address, int size) {
  uint64_t set_idx = get_set_index(address);
  uint64_t tag_idx = get_tag_index(address);

  switch (sig) {
  case 'I':
    return;
  case 'L':
    do_load(set_idx, tag_idx);
    return;
  case 'S':
    do_store(set_idx, tag_idx);
    return;
  case 'M':
    do_modify(set_idx, tag_idx);
    return;
  default:
    return;
  }
};

int main(int argc, char *argv[]) {
  // TODO -s -E -b -t parsing

  char *trace_path = NULL;
  int verbose = 0;

  int opt;

  while ((opt = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
    switch (opt) {
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      trace_path = optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      printf("Usage: %s -s <s> -E <E> -b <b> -t <trace>\n", argv[0]);
      return 0;
    default:
      return 1;
    }
  }

  if (s <= 0 || E <= 0 || b <= 0 || trace_path == NULL) {
    fprintf(stderr, "Invalid or missing arguments\n");
    return 1;
  }

  printf("s=%d E=%d b=%d trace=%s verbose=%d\n", s, E, b, trace_path, verbose);

  // simulation을 위한 자료구조 초기화.

  // file.
  FILE *fp = fopen(trace_path, "r");
  if (!fp) {
    perror("fopen");
    return 1;
  }

  char input_line[256];
  t -= (s + b);

  // initialize.
  initialize(s, E, b);

  // while loop.
  while (fgets(input_line, sizeof(input_line), fp) != NULL) {
    char sig;
    uint64_t address;
    int size;

    if (sscanf(input_line, " %c %" SCNx64 ",%d", &sig, &address, &size) != 3) {
      continue;
    }

    if (sig == 'I')
      continue;

    simulate(sig, address, size);
  }

  fclose(fp);

  printSummary(hits, misses, evictions);

  // destory.
  destory(s, E);

  return 0;
}
