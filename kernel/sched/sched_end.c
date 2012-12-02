#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

void remove_task(task_node_list *list, task_node *node) {
    /* Remove from end/middle/beginning of list */
    if (node->next == NULL && node->prev) {
        node->prev->next = NULL;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    else {
        node->next->prev = NULL;
    }

    /* Remove from head/tail of list->*/
    if (list->head == node) {
        list->head = node->next;
    }
    if (list->tail == node) {
        list->tail = node->prev;
    }
}

void free_task(task_node *node) {
    if (node->task->periodic_node) {
        kfree(node->task->periodic_node);
    }
    free(node->task->stack_limit);
    kfree(node->task);
    kfree(node);
}

/* Abort a periodic task */
void abort(void) {
    curr_task->task->abort = 1;
    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

void end_task(void) {
    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

/* Called by svc_handler */
void svc_end_task(void) {
    if (curr_task->task->stack_limit > curr_task->task->stack_top) {
        panic_print("Task has overflowed its stack.");
    }

    remove_task(&task_list, curr_task);

    curr_task->prev = NULL;
    curr_task->next = NULL;

    /* Periodic (but only if not aborted) */
    if (!curr_task->task->abort && curr_task->task->period) {
        curr_task->task->running = 0;

        /* Reset stack */
        curr_task->task->stack_top = curr_task->task->stack_base;
    }
    else {
        task_node *node = task_to_free;

        /* curr_task->next set to NULL after task switch */
        if (node != NULL) {
            while (node->next != NULL) {
                node = node->next;
            }
            node->next = curr_task;
            node = node->next;
        }
        else {
            task_to_free = curr_task;
            node = task_to_free;
        }
    }

    switch_task(NULL);
}
