#ifndef KERNEL_SEMAPHORE_H_INCLUDED
#define KERNEL_SEMAPHORE_H_INCLUDED

#include <stdint.h>

struct task_node;

struct semaphore {
        uint8_t             lock;
        struct task_node   *held_by;
        struct task_node   *waiting;
};

typedef struct semaphore semaphore;

void init_semaphore(volatile struct semaphore *semaphore);
void acquire(volatile struct semaphore *semaphore);
void release(volatile struct semaphore *semaphore);

#endif
