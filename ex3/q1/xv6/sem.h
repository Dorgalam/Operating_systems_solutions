struct sem {
    char name[6];
    int owner_pid;
    struct spinlock sslock; // single semaphore lock
    int count;
    int ref;
    int maxVal;
    char linked;
    char waitingForDeath;
};