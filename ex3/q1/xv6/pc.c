#include "types.h"
#include "stat.h"
#include "user.h"
#include "pc_def.c"

int main(int argc, char *argv[]) {
  int prodNum = atoi(argv[1]), 
    consNum = atoi(argv[2]), 
    msgNumPerProd = atoi(argv[3]),
    i, waitcd, buffb, buffa, write, j, totald;
  waitcd = sem_open("waitc", 0, 0); // exists simply to be closed, children will wait until it is unlinked and parent will close it once all children were created
  buffa = sem_open("buffa", MAX_MSG, MAX_MSG); // available spots in the buff
  buffb = sem_open("buffb", 0, MAX_MSG); // filled spots in the buff
  totald = sem_open("total", prodNum * msgNumPerProd, prodNum * msgNumPerProd); // cons processes run while this sem is available 
  write = sem_open("write", 1, 1); // the right to read/write to buffer

  // loop creating prod processes
  for(i = 0; i < prodNum; ++i) {
    if (fork() == 0) {
      sem_close(waitcd);
      sem_unlink("waitc"); // gets blocked until parent finally closes
      for(j = 0; j < msgNumPerProd; j++) {
        sem_wait(buffa);
        sem_wait(write);
        write_prod_msg();
        sem_post(write);
        sem_post(buffb);
      }
      exit();
    }
  }

  // loop creating cons processes
  for(i = 0; i < consNum; ++i) {
    if (fork() == 0) {
      sem_close(waitcd);
      sem_unlink("waitc"); // gets blocked until parent finally closes
      while(sem_try_wait(totald) > -1) {
        sem_wait(buffb);
        sem_wait(write);
        write_cons_msg();
        sem_post(write);
        sem_post(buffa);
      }
      exit();   
    }
  }
  write_parent_msg();
  sem_close(waitcd); // child processes may start doing their thing
  for(i = 0; i < consNum + prodNum; ++i)
    wait(); // wait for all to finish and exit so we won't have any wandering zombies
  sem_close(buffa);
  sem_unlink("buffa");
  sem_close(buffb);
  sem_unlink("buffb");
  sem_close(totald);
  sem_unlink("total");
  sem_close(write);
  sem_unlink("write");
  exit();
}


