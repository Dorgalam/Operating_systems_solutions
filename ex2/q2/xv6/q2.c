#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_data.h"

int
main(int argc, char *argv[])
{
  struct proc_data myproc;
  printf(1, "pid:%d ppid:%d pppid:%d\n", getpid(), getppid(), getpppid());
  printf(1, "pid from proc_data: %d\n", get_proc_data(1, &myproc));
  exit();
}
