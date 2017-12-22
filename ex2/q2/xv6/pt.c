#include "types.h"
#include "stat.h"
#include "user.h"

char *newArgv[3];

int
main(int argc, char *argv[])
{
  int N, fd[2];
  char buff[2];
  if(argc < 2 || (N = atoi(argv[1]))  < 1) {
   printf(2, "no valid pid was supplied\n");
   exit();
  }
  newArgv[0] = "./ps";
  newArgv[1] = "1";
  newArgv[2] = 0;
  pipe(fd);
  for(int i = 0; i < N; ++i) {
    if(fork() == 0) {
      close(fd[1]);
      read(fd[0], buff, 1);
      exit();
    }
  }
  if (fork() == 0) {
    exec("./ps", newArgv);
    close(fd[1]);
    exit();
  }
  close(fd[1]);
  for(int i = 0; i < N + 1; ++i)
    wait();
  exit();
}
