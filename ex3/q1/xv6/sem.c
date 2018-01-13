#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "sem.h"
#include "mmu.h"
#include "proc.h"


struct {
  struct spinlock gslock;
  struct sem sem[NSEM];
} stable;


void
seminit(void)
{
  initlock(&stable.gslock, "stable");
}

struct sem *semdup(struct sem *s) {
  acquire(&stable.gslock);
  ++s->ref;
  release(&stable.gslock);
  return s;
}

struct sem *sem_open(char *name, int init, int maxVal) {
  struct sem *s, *spot = 0;
  acquire(&stable.gslock);
  for(s = stable.sem; s < stable.sem + NSEM; s++) {
    if (s->ref > 0 && strncmp(s->name, name, SEMNAME) == 0) {
      s->ref++;
      release(&stable.gslock);
      return s;
    } else if (s->linked == 0 && !spot) {
      spot = s;
    }
  }
  release(&stable.gslock);
  if (spot) {
    initlock(&spot->sslock, name);
    acquire(&stable.gslock);
    safestrcpy(spot->name, name, SEMNAME);
    spot->linked = 1;
    spot->ref = 1;
    spot->count = init;
    spot->maxVal = maxVal;
    spot->owner_pid = myproc()->pid;
    spot->waitingForDeath = 0;
    release(&stable.gslock);
    return spot;
  }
  return 0;
}

int sem_close(struct sem *s) {
  acquire(&stable.gslock);
  if (--s->ref == 0 && s->waitingForDeath)
    wakeup(&stable.gslock);
  release(&stable.gslock);
  return 0;
}

int sem_wait(struct sem *s) {
  struct proc *curproc = myproc();
  acquire(&s->sslock);
  for (;;) {
    if (s->count > 0) {
      s->count--;
      release(&s->sslock);
      return 0;
    }
    if (curproc->killed) {
      release(&s->sslock);
      return -1;
    }
    sleep(s, &s->sslock);
  }
  return 0;
}

int sem_try_wait(struct sem *s) {
  int res;
  acquire(&s->sslock);
  if (s->count > 0) {
    --s->count;
    res = 0;
  } else {
    res = -1;
  }
  release(&s->sslock);
  return res;
}

int sem_post(struct sem *s) {
  int res = 0;
  acquire(&s->sslock);
  if (s->count == s->maxVal) 
    res = -2;
  else if (++s->count == 1)
    wakeup(s);
  release(&s->sslock);
  return res;
}

int sem_reset(struct sem *s, int newVal, int newMaxVal) {
  int oldVal;
  if ((myproc()->pid != s->owner_pid) || 
    (newMaxVal >= 0 && newMaxVal < s->maxVal) || 
    (newMaxVal < 0 && newVal < 0))
    return -1;
  if (newVal >= 0) {
    acquire(&s->sslock);
    oldVal = s->count;
    s->count = newVal;
    release(&s->sslock);
    if (newVal > 0 && oldVal == 0) 
      wakeup(s);
  }
  if (newMaxVal > s->maxVal) {
    acquire(&s->sslock);
    s->maxVal = newMaxVal;
    release(&s->sslock);
  }
  return 0;
}
int sem_unlink(char *name) {
  struct sem *s, *found = 0;
  acquire(&stable.gslock);
  for(s = stable.sem; s < &stable.sem[NSEM] && !found; s++) 
    if (strncmp(s->name, name, SEMNAME) == 0)
      found = s;
  release(&stable.gslock);
  if (!found)
    return -1;
  acquire(&stable.gslock);
  found->waitingForDeath = 1;
  for (;;) {
    if (found->ref == 0) {
      found->linked = 0;
      release(&stable.gslock);
      return 0;
    }
    if (myproc()->killed) {
      release(&stable.gslock);
      return -1;
    }
    sleep(&stable, &stable.gslock);
  }
  return 0;
}