#define MAX_CHILDREN 10

struct proc_data {
  uint pid;
  char name[16];
  uint chld_num;
  uint pids[MAX_CHILDREN];
};