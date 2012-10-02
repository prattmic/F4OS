#include <stddef.h>

#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include "sched_internals.h"

task_node * volatile task_to_free = NULL;

void kernel_task(void) {
    /* Does cleanup that can't be done from outside a task (ie. in an interrupt) */

    while (1) {
        while (task_to_free != NULL) {
            task_node *node = task_to_free;

            task_to_free = task_to_free->next;

            /* Free abandoned semaphores */
            for (int i = 0; i < HELD_SEMAPHORES_MAX; i++) {
                if (node->task->held_semaphores[i]) {
                    release(node->task->held_semaphores[i]);
                }
            }

            free_task(node);
        }

        SVC(SVC_YIELD);
    }
}
