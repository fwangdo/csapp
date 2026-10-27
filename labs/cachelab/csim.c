#include "cachelab.h"
#include "inttypes.h"
#include "stdint.h"
#include "stdio.h"
#include <stdint.h>

// stats
int hits;
int misses;
int evictions;

char *trace_path;

// structures

// funcs. TO-DO.
void simulate(char sig, uint64_t address, int size) {}

int main(int argc, char *argv[]) {
  // TODO -s -E -b -t parsing
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
