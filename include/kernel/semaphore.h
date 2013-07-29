#ifndef KERNEL_SEMAPHORE_H_INCLUDED
#define KERNEL_SEMAPHORE_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define HELD_SEMAPHORES_MAX         CONFIG_HELD_SEMAPHORES_MAX

struct task_t;
typedef struct task_t task_t;

struct semaphore {
        uint8_t lock;
        task_t  *held_by;
        task_t  *waiting;
};

typedef struct semaphore semaphore;

struct task_semaphore_data {
    struct semaphore   *held_semaphores[HELD_SEMAPHORES_MAX];
    struct semaphore   *waiting;
};

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

/* Setup semaphore data structure for a new task */
void task_semaphore_setup(task_t *task);

/**
 * Semaphore service call handler
 * Should only be called by global SVC handler.  This takes va_args for the
 * semaphore service calls and returns the result of the service call.
 *
 * @param svc_number    Service call number.  Must be a semaphore service call
 * @param va_args       Arguments for service call
 * @returns Return value of service call
 */
int semaphore_service_call(uint32_t svc_number, ...);

#endif
