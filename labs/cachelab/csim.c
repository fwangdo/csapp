#include "cachelab.h"

// stats
int hits;
int misses;
int evictions;

// structures

char parse_char(char *line) {}

int parse_left(char *line) {}

int parse_right(char *line) {}

void simulate(char sig, int left, int right) {}

int main(int argc, char *argv[]) {
  // while loop.
  while (1) {
    char *line = get_line();
    // line.
    char sig = parse_char(line);
    int left = parse_left(line);
    int right = parse_right(line);

    simulate(sig, left, right);
  }

  printSummary(hits, misses, evictions);
  return 0;
}
