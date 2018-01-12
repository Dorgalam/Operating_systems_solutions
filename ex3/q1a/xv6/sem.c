#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "sem.h"

struct {
  struct spinlock gslock;
  struct sem sem[NSEM];
} stable;


struct sem *sem_open(char *name, int init, int maxVal) {
  return 0;
}
int sem_close(struct sem *s) {
  return 0;

}
int sem_wait(struct sem *s) {
  return 0;

}
int sem_try_wait(struct sem *s) {
  return 0;

}
int sem_post(struct sem *s) {
  return 0;

}
int sem_reset(struct sem *s, int newVal, int newMaxVal) {
  return 0;

}
int sem_unlink(char *name) {
  return 0;

}