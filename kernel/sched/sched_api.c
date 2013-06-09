#include <kernel/svc.h>
#include <kernel/sched.h>
#include "sched_internals.h"

/* Implements general scheduler API functions */

int task_compare(task_t *task1, task_t *task2) {
    /* Anything is better than the NULL task */
    if (!task1) {
        return -1;
    }

    if (!task2) {
        return 1;
    }

    struct task_ctrl *t1 = get_task_ctrl(task1);
    struct task_ctrl *t2 = get_task_ctrl(task2);

    if (t1->priority > t2->priority) {
        return 1;
    }
    else if (t2->priority > t1->priority) {
        return -1;
    }

    return 0;
}

int task_switch(task_t *task) {
    int ret;
    task_ctrl *t = get_task_ctrl(task);

    /* If already in interrupt space, switch directly */
    if (IPSR()) {
        ret = coop_task_switch(t);
    }
    /* Otherwise, make a service call */
    else {
        ret = SVC_ARG(SVC_TASK_SWITCH, t);
    }

    return ret;
}
