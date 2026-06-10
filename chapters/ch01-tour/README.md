# Chapter 1: A Tour of Computer Systems

Goal: compile and inspect the simplest C program through the toolchain.

Commands:

```sh
cc -Wall -Wextra -O0 -o hello hello.c
./hello
cc -E hello.c > hello.i
cc -S hello.c
cc -c hello.c
```
