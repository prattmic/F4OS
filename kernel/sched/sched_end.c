#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>

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
    free(node->task->stack_limit);
    kfree(node->task);
    kfree(node);
}

void end_task(void) {
    __asm__("mrs    %[ghetto], msp"
            :[ghetto] "=r" (ghetto_sp_save)::);
    _svc(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

void end_periodic_task(void) {
    __asm__("mrs    %[ghetto], msp"
            :[ghetto] "=r" (ghetto_sp_save)::);
    _svc(SVC_END_PERIODIC_TASK);    /* Shouldn't return (to here, at least) */
}
