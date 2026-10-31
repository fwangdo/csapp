#include "cachelab.h"
#include "inttypes.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

// stats
int hits;
int misses;
int evictions;

char *trace_path;

// structures
typedef struct line {
  short valid;
  int tag;     // TODO: select proper data type.
  int block[]; // TODO
} line_t;

typedef struct set {
  int set_idx;
  line_t lines[];
} set_t;

set_t cache[30];

// funcs. TO-DO.
void simulate(char sig, uint64_t address, int size) {}

int main(int argc, char *argv[]) {
  // TODO -s -E -b -t parsing
  int s = -1;
  int E = -1;
  int b = -1;
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

  char line[256];

  // while loop.
  while (fgets(line, sizeof(line), fp) != NULL) {
    char sig;
    uint64_t address;
    int size;

    if (sscanf(line, " %c %" SCNx64 ",%d", &sig, &address, &size) != 3) {
      continue;
    }

    if (sig == 'I')
      continue;

    simulate(sig, address, size);
  }

  fclose(fp);

  printSummary(hits, misses, evictions);
  return 0;
}
