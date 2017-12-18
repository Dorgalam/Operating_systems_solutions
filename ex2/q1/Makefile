# Makefile for ex2-q1 winter 2018A
CFLAGS = -Wall
LDFLAGS = -lm # not really needed for this exercise
CC = gcc -std=c99
ECHO = echo "going to compile for target $@"
OS := $(shell uname)
ifneq (,$(findstring CYGWIN_NT,$(OS)))
EXT = .exe
else
EXT =
endif
PROG1 = polygon$(EXT)

all: writer polygon

polygon: main.c
	CC -o polygon main.c

debug_polygon: main.c
	CC -g -o polygon main.c -I.

writer: writer.c
	CC -o writer writer.c

clean: 
	rm -rf polygon polygon.dSYM *.o *.log
test: 
	./$(PROG1) < in.txt 
