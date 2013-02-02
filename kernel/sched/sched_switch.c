#include <stddef.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

void switch_task(task_node *node) {
    /* Optionally pass node to switch to, otherwise pass NULL */

    /* Rate monotonic scheduling
     * Always runs the highest priority task,
     * which will be the head of the list, as
     * it is kept sorted.  Round-robin through
     * equal priority tasks. */

    if (node == NULL) {

        /* Temporary fix to ensure tim2_handler doesn't modify the task_list */
        __asm__("cpsid  i");

        node = task_list.head;
        curr_task = node;
        if (node == NULL) {
            /* Uh-oh, no tasks! */
            __asm__("cpsie  i");
            panic_print("No tasks to run.");
        }

        /* As a workaround for lack of MPU support, check if the 
         * stack of the task we are switching from has overflowed */
        if (node->task->stack_limit > node->task->stack_top) {
            __asm__("cpsie  i");
            panic_print("Task (0x%x, fptr: 0x%x) has overflowed its stack. stack_top: 0x%x stack_limit: 0x%x", node->task, node->task->fptr, node->task->stack_top, node->task->stack_limit);
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
    }
    else {
        curr_task = node;
    }

    /* mpu_stack_set(node->task->stack_base);   Sigh...maybe some day */

    if (node->task->running) {
        enable_psp(node->task->stack_top);
        return;
    }
    else {
        node->task->running = 1;

        create_context(node->task, &end_task);

        enable_psp(node->task->stack_top);
        return;
    }
}

/* Updates ticks in all periodic tasks */
void rtos_tick(void) {
    task_node *node = periodic_task_list.head;

    while (node) {
        if (node->task->ticks_until_wake == 0) {
            if (!node->task->running) {
                append_task(&task_list, node->task->task_list_node);
            }
            node->task->ticks_until_wake = node->task->period;
        }
        else {
            node->task->ticks_until_wake--;
        }

        node = node->next;
    }
}
