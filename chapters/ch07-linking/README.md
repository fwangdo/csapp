# Chapter 7: Linking

Build a tiny multi-file program and inspect symbols.

Commands:

```sh
make
nm -g app
otool -L app 2>/dev/null || ldd app
make clean
```
