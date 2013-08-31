#include <stddef.h>
#include <kernel/power.h>
#include <kernel/fault.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

struct list free_task_list = INIT_LIST(free_task_list);

void kernel_task(void) {
    /* Does cleanup that can't be done from outside a task (ie. in an interrupt) */

    while (!list_empty(&free_task_list)) {
        struct list *element = list_pop(&free_task_list);
        struct task_ctrl *task = list_entry(element, struct task_ctrl, free_task_list);

        /* Free abandoned semaphores */
        for (int i = 0; i < HELD_SEMAPHORES_MAX; i++) {
            struct task_semaphore_data *sem_data = &get_task_t(task)->semaphore_data;

            if (sem_data->held_semaphores[i]) {
                release(sem_data->held_semaphores[i]);
            }
        }

        free_task(task);
    }
}

void sleep_task(void) {
    /* Run when there is nothing else to run */
    while (1) {
        arch_wait_for_interrupt();
    }
}
