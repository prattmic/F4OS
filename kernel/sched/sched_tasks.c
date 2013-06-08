#include <kernel/sched.h>

#include "sched_internals.h"

volatile uint32_t total_tasks = 0;

/* Is task in task_list? */
uint8_t task_exists(task_ctrl *task) {
    task_ctrl *node;

    list_for_each_entry(node, &runnable_task_list, runnable_task_list) {
        if (node == task) {
            return 1;
        }
    }

    return 0;
}
