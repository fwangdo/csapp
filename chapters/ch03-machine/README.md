# Chapter 3: Machine-Level Representation of Programs

Use `inspect.c` to connect C, assembly, and debugger output.

Commands:

```sh
cc -Wall -Wextra -O0 -g -o inspect inspect.c
cc -Wall -Wextra -O2 -S inspect.c
objdump -d inspect
lldb ./inspect
```

Then move to Bomb Lab and Attack Lab.
