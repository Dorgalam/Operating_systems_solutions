#include "types.h"
#include "stat.h"
#include "user.h"

void print_test(char *msg, char res) {
  printf(1, "\t%s %s\n", msg, res ? "OK" : "FAILED");
}

int
main(int argc, char *argv[])
{
  if (fork() == 0) {
    printf(1, "sem_open:\n");
    int sd2, sd3;
    sem_open("sem", 1, 4);
    sd2 = sem_open("sem", 0, 4);
    sd3 = sem_open("sem", 0, 4);
    sd3 = sem_open("sem", 0, 4);
    sd3 = sem_open("sem", 0, 4);
    sd3 = sem_open("sem", 0, 4);
    print_test("it shouldnt open a new sem when the given name exists", sem_try_wait(sd2) == 0);
    print_test("it should allow you to open more then 5 sems", sd3 == -1);
    exit();
  }
  wait();
  int psd = sem_open("dad", 2, 2);
  if (fork() == 0) {
    printf(1, "sem_close:\n");
    print_test("it should allow closing sd got from parent", sem_close(psd) == 0);
    print_test("it shouldnt allow closing illegal sd", (sem_close(4) == -1) && (sem_close(-2) == -1 )&& (sem_close(10) == -1));
    exit();
  }
  wait();
  if (fork() == 0) {
    printf(1, "sem_open:\n");
    print_test("it should allow the process to continue when sem count is more than 0", (sem_try_wait(psd) >= 0) && (sem_try_wait(psd) >= 0));
    print_test("it should block process from continuing when no sems are available", sem_try_wait(psd) < 0);
    if (fork() == 0) {
      sem_post(psd);
      exit();
    }
    wait();
    print_test("it should allow process to continue after another process posted a sem", sem_try_wait(psd) >= 0);
    exit();
  }
  wait();
  if (fork() == 0) {
    printf(1, "sem_post:\n");
    if (fork() == 0) {
      sleep(20);
      sem_post(psd);
      exit();
    }
    sem_wait(psd);
    print_test("it should go to sleep when sem isnt available and then woken up by post", 1);
    sem_post(psd);
    sem_post(psd);
    print_test("it should return error code -2 when trying to incerement past maxVal", sem_post(psd) == -2);
    wait();
    exit();
  }
  wait();
  if (fork() == 0) {
    printf(1, "sem_reset:\n");
    print_test("it should not allow reseting a sem not owned by me", sem_reset(psd, 1, 3) < 0);
    int srsd = sem_open("sem_reset", 0, 4);
    print_test("it should not allow changing invalid values", (sem_reset(srsd, 0, 3) < 0) && sem_reset(srsd, -1, -1) < 0);
    exit();
  }
  wait();
  if (fork() == 0) {
    printf(1, "sem_unlink:\n");
    sem_close(psd);
    sem_unlink("dad");
    print_test("it should continue after parent closes the sem", 1);
  }
  sleep(20);
  sem_close(psd);
  wait();
  exit();
}
