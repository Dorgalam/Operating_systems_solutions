
Dor Galam - 311232268
Operating Systems 2018A - exercise 3 parts A and B

Semaphores used in pc.c:

1. waitc - exists simply to be closed, children will wait until it is unlinked and parent will close it once all children were created
2. buffa - available spots in the buff
3. buffb - filled spots in the buff
4. total - cons processes run while this sem is available 
5. write - the right to read/write to buffer

How do cons processes know when to finish?

I created an extra semaphore called total which holds the value of the multiplication of prod * msgPerProd
Every cons process then loops while sem_try_wait is non negative, which means that once the sem count
reaches 0, all of the processes will exit the loop, otherwise, they will enter the loop and decrement the
value of total until it is 0.