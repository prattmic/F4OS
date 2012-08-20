#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

void init_semaphore(volatile struct semaphore *semaphore);
void spin_acquire(volatile struct semaphore *semaphore);
void acquire(volatile struct semaphore *semaphore);
void release(volatile struct semaphore *semaphore);

#endif
