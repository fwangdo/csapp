# Chapter 5: Optimizing Program Performance

Use `benchmark.c` to measure a small kernel before optimizing anything. Then
compare with Performance Lab.

Commands:

```sh
cc -Wall -Wextra -O0 -o benchmark-O0 benchmark.c
cc -Wall -Wextra -O2 -o benchmark-O2 benchmark.c
./benchmark-O0
./benchmark-O2
```
