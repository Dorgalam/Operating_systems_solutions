#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(int argc, char **argv) {
  int fileOut;
  if (argc > 1 && strcmp(argv[1], "std") != 0) {
    if ((fileOut = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1) {
      exit(EXIT_FAILURE);
    } else {
      close(1);
      dup(fileOut);
      close(fileOut);
    }
  }
  char ch;
  while ((ch = getchar()) != EOF) 
    putchar(ch);
}