#ifndef KERNEL_SEMAPHORE_H_INCLUDED
#define KERNEL_SEMAPHORE_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

struct task_node;

struct semaphore {
        uint8_t             lock;
        struct task_ctrl   *held_by;
        struct task_ctrl   *waiting;
};

typedef struct semaphore semaphore;

void acquire(volatile struct semaphore *semaphore);
void acquire_for_free(volatile struct semaphore *semaphore);
void release(volatile struct semaphore *semaphore);

static inline void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    semaphore->waiting = NULL;
}

#define INIT_SEMAPHORE  {\
    .lock = 0,          \
    .held_by = NULL,    \
    .waiting = NULL,    \
}

#endif
