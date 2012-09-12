#include <stdint.h>
#include <stddef.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <kernel/fault.h>

#include <kernel/semaphore.h>

void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    semaphore->waiting = NULL;
}

void acquire(volatile struct semaphore *semaphore) {
    if (!task_switching) {
        semaphore->lock = 1;
        semaphore->held_by = (task_node *) 0x0badf00d;
        return;
    }

    int success = 0;

    while (!success) {
        success = SVC_ARG(SVC_ACQUIRE, (void *) semaphore);
    }
}

/* Called by svc_handler */
int get_lock(volatile struct semaphore *semaphore) {
    if (!semaphore->lock) {
        semaphore->lock = 1;
        semaphore->held_by = curr_task;
        return 1;
    }
    else {
        if (semaphore->held_by != NULL) {
            /* Add to waitlist if higher priority */
            if (semaphore->waiting) {
                if (semaphore->waiting->task->priority < curr_task->task->priority) {
                    semaphore->waiting = curr_task;
                }
            }
            else {
                semaphore->waiting = curr_task;
            }

            return 0;
        }
        else {
            panic_print("Semaphore not available, but held_by unset.");
        }
    }

    return 0;
}

void release(volatile struct semaphore *semaphore) {
    if (!task_switching) {
        semaphore->lock = 0;
        semaphore->held_by = NULL;
        return;
    }

    SVC_ARG(SVC_RELEASE, (void *) semaphore);
}
