#include <stddef.h>

#include <kernel/sched.h>
#include "sched_internals.h"

task_node * volatile task_to_free = NULL;

void kernel_task(void) {
    /* Does cleanup that can't be done from outside a task (ie. in an interrupt) */

    while (1) {
        while (task_to_free != NULL) {
            task_node *node = task_to_free;

            task_to_free = task_to_free->next;

            free_task(node);
        }

        _svc(SVC_YIELD);
    }
}
