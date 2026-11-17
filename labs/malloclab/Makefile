#
# Students' Makefile for the Malloc Lab
#
TEAM = bovik
VERSION = 1
HANDINDIR = /afs/cs.cmu.edu/academic/class/15213-f01/malloclab/handin

CC = gcc
CFLAGS = -Wall -O2

# Keep native objects separate from the supplied 32-bit ELF objects.
BUILD_DIR = build
OBJS = $(addprefix $(BUILD_DIR)/,mdriver.o mm.o memlib.o fsecs.o fcyc.o clock.o ftimer.o)

.PHONY: test clean handin

mdriver: $(OBJS)
	$(CC) $(CFLAGS) -o mdriver $(OBJS)

$(BUILD_DIR)/%.o: %.c $(wildcard *.h) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

test: mdriver
	./mdriver -V

handin:
	cp mm.c $(HANDINDIR)/$(TEAM)-$(VERSION)-mm.c

clean:
	rm -f $(OBJS) mdriver
	rmdir $(BUILD_DIR) 2>/dev/null || true


