#
# Student makefile for Cache Lab
# macOS uses the native transpose tester; Linux retains the official tester.
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64
PYTHON ?= python3
CLANG ?= clang
NATIVE_CFLAGS = -g -Wall -Werror -std=c99 -O0 -fsanitize=address -I.
HOST_OS := $(shell uname -s)

all: csim test-trans tracegen
	# Generate a handin tar file each time you compile
	-tar -cvf ${USER}-handin.tar  csim.c trans.c 

csim: csim.c cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o csim csim.c cachelab.c -lm 

ifeq ($(HOST_OS),Darwin)
test-trans: tools/native_test_trans.c tools/native_trans.c trans-native.o cachelab.c cachelab.h Makefile
	$(CLANG) $(NATIVE_CFLAGS) -DNATIVE_TRANS_LIBRARY -o $@ tools/native_test_trans.c tools/native_trans.c cachelab.c trans-native.o
else
test-trans: test-trans.c trans.o cachelab.c cachelab.h Makefile
	$(CC) $(CFLAGS) -o test-trans test-trans.c cachelab.c trans.o 
endif

trans-native.o: trans.c cachelab.h Makefile
	$(CLANG) $(NATIVE_CFLAGS) -fsanitize-coverage=trace-pc,trace-loads,trace-stores -c trans.c -o $@

tracegen: tracegen.c trans.o cachelab.c
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o cachelab.c

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f *.tar
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker

# Native development tools; the original Linux grading targets stay available.
.PHONY: all clean check-native-tools test-native-a test-native-b

check-native-tools:
	$(PYTHON) -m unittest discover -s tools -p test_native_tools.py -v

test-native-a: csim
	$(PYTHON) tools/test_native.py csim

test-native-b:
	$(PYTHON) test_trans.py
