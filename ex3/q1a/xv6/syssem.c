#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "sem.h"

// Fetch the nth word-sized system call argument as a semaphore descriptor
// and return both the descriptor and the corresponding struct file.
static int
argsd(int n, int *psd, struct sem **ps)
{
  int sd;
  struct sem *s;

  if(argint(n, &sd) < 0)
    return -1;
  if(sd < 0 || sd >= NOSEM || (s = myproc()->osem[sd]) == 0)
    return -1;
  if(psd)
    *psd = sd;
  if(ps)
    *ps = s;
  return 0;
}

int 
sys_sem_open(void)
{
  char *name;
  int init, maxVal, sd;
  struct sem *sem;
  struct proc *curproc;
  if (argstr(0, &name) < 0 || argint(1, &init) < 0 || argint(2, &maxVal) < 0) 
    return -1;
  if ((sem = sem_open(name, init, maxVal)) == 0) 
    return -1;
  curproc = myproc();
  for(sd = 0; sd < NOSEM; sd++){
    if(curproc->osem[sd] == 0){
      curproc->osem[sd] = sem;
      return sd;
    }
  }
  return -1;
}

int 
sys_sem_close(void)
{
  int sd;
  struct sem *s;
  if (argsd(0, &sd, &s) < 0) 
    return -1;
  myproc()->osem[sd] = 0;
  sem_close(s);
  return 0;
}

int 
sys_sem_wait(void)
{
  struct sem *s;
  if (argsd(0, 0, &s) < 0) 
    return -1;
  return sem_wait(s);
}

int 
sys_sem_try_wait(void)
{
  struct sem *s;
  if (argsd(0, 0, &s) < 0) 
    return -1;
  return (sem_try_wait(s));
}

int 
sys_sem_post(void)
{
  struct sem *s;
  if (argsd(0, 0, &s) < 0) 
    return -1;
  return sem_post(s);
}

int 
sys_sem_reset(void)
{
  int newVal, newMaxVal;
  struct sem *s;
  if (argsd(0, 0, &s) < 0 || argint(1, &newVal) < 0 || argint(2, &newMaxVal) < 0) 
    return -1;
  sem_reset(s, newVal, newMaxVal);
  return 0;

} 

int 
sys_sem_unlink(void)
{
  char *name;
  if(argstr(0, &name) < 0)
    return -1;
  sem_unlink(name);
  return 0;

}