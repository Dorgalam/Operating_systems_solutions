#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_data.h"

void print_proc_data(struct proc_data* pdata);
void print_proc_and_children(int pid);

int
main(int argc, char *argv[])
{
  int pidarg;
  struct proc_data p;
  if(argc < 2 || (pidarg = atoi(argv[1]))  < 1) {
   printf(2, "no valid pid was supplied\n");
   exit();
  }
  get_proc_data(getpid(), &p);
  printf(1, "%s pid=%d argument=%d\n", p.name, p.pid, pidarg);
  printf(1, "Name PID #Children Children PIDs\n");
  print_proc_and_children(pidarg);
  exit();
}

void 
print_proc_and_children(int pid) {
  struct proc_data p;
  get_proc_data(pid, &p);
  print_proc_data(&p);
  for(int i = 0; i < p.chld_num; ++i)
    print_proc_and_children(p.pids[i]);
}

void 
print_proc_data(struct proc_data* pdata)
{
     int i;
     printf(1, "%s %d #children: %d  children PIDs: ",
            pdata->name, pdata->pid, pdata->chld_num);
     for (i=0; i < pdata->chld_num; i++)
          printf(1, "%d ", pdata->pids[i]);
     printf(1,"\n");
}