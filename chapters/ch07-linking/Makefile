CC ?= cc
CFLAGS ?= -Wall -Wextra -O0 -g

app: main.o lib.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f app *.o

.PHONY: clean
