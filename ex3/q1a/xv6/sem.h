struct sem {
    char *name;
    int owner_pid;
    struct spinlock sslock; // single semaphore lock
    int count;
    int ref;
};