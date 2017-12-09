# Makefile for ex1-q1 winter 2018A
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

polygon: main.c
	gcc -o polygon main.c -I.

debug: main.c
	gcc -g -o polygon main.c -I.

all: 
	./$(PROG1) test
clean: 
	rm -rf polygon polygon.dSYM *.o *.log
test: 
	./$(PROG1) < in.txt > out.log