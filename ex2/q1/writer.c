#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

void flushBuff();

// Makes the program a bit more complex but this make sures
// the buffer is flushed before terminating this program, even if
// it was terminated by an interrupt

void flushBuff() {
  fflush(stdout);
  exit(EXIT_SUCCESS); 
}

int main(int argc, char **argv) {
  int fileOut;
  signal(SIGINT, flushBuff);
  if (argc > 1 && strcmp(argv[1], "std") != 0) {
    close(1);
    if (open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0666) == -1) 
      exit(EXIT_FAILURE);
  }
  char ch;
  while ((ch = getchar()) != EOF) 
    putchar(ch);
}