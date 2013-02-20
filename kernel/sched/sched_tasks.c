#include <kernel/sched.h>

#include "sched_internals.h"

volatile uint32_t total_tasks = 0;

/* Is task in task_list? */
uint8_t task_exists(task_node *task) {
    task_node *node = task_list.head;

    while (node) {
        if (node == task) {
            return 1;
        }
        node = node->next;
    }

    return 0;
}
