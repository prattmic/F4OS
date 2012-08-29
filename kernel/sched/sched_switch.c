#include <stddef.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

void switch_task(void) {
    /* Rate monotonic scheduling
     * Always runs the highest priority task,
     * which will be the head of the list, as
     * it is kept sorted.  Round-robin through
     * equal priority tasks. */

    /* Temporary fix to ensure tim2_handler doesn't modify the task_list */
    __asm__("cpsid  i");

    task_node *node = task_list.head;
    curr_task = node;
    if (curr_task == NULL) {
        /* Uh-oh, no tasks! */
        __asm__("cpsie  i");
        panic_print("No tasks to run.");
    }

    /* As a workaround for lack of MPU support, check if the 
     * stack of the task we are switching from has overflowed */
    if (node->task->stack_limit > node->task->stack_top) {
        __asm__("cpsie  i");
        panic_print("Task has overflowed its stack.");
    }

    /* Don't bother moving this if it is the only (high priority) task */
    if (node->next && node->task->priority <= node->next->task->priority) {
        /* Move to end of priority */
        task_node *prev = node;
        task_node *next = node->next;
        while (next && node->task->priority == next->task->priority) {
            prev = next;
            next = next->next;
        }

        /* Found next priority level */
        if (next) {
            task_list.head = node->next;
            node->next->prev = NULL;

            prev->next = node;
            node->prev = prev;

            next->prev = node;
            node->next = next;
        } 
        /* Reached end of list without priority changing, place on end. */
        else {
            if (node->next) {
                task_list.head = node->next;
                node->next->prev = NULL;
            }

            node->next = NULL;
            node->prev = task_list.tail;
            task_list.tail->next = node;
            task_list.tail = node;
        }
    }

    __asm__("cpsie  i");

    /* mpu_stack_set(node->task->stack_base);   Sigh...maybe some day */

    if (node->task->running) {
        uint32_t *psp_addr = node->task->stack_top;
        enable_psp(psp_addr);
        return;
    }
    else {
        node->task->running = 1;

        if (node->task->period) {
            create_context(node->task, &end_periodic_task);
        }
        else {
            create_context(node->task, &end_task);
        }
        enable_psp(node->task->stack_top);
        return;
    }
}
