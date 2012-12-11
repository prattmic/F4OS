#include <kernel/sched.h>

#include "sched_internals.h"

/* This is approximate because there is no guarantee that it
 * won't change in the middle of counting */
int approx_num_tasks(void) {
    int tasks = 0;

    task_node *task = task_list.head;
    while (task) {
        tasks++;
        task = task->next;
    }

    task = periodic_task_list.head;
    while (task) {
        if (task->task && !task->task->running) {
            tasks++;
        }
        task = task->next;
    }

    return tasks;
}
