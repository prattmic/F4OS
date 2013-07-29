#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <mm/mm.h>
#include <dev/resource.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
static int register_task(task_ctrl *task_ptr, int periodic) __attribute__((section(".kernel")));

struct list runnable_task_list = INIT_LIST(runnable_task_list);
struct list periodic_task_list = INIT_LIST(periodic_task_list);

DEFINE_INSERT_TASK_FUNC(runnable_task_list);
DEFINE_INSERT_TASK_FUNC(periodic_task_list);

volatile uint32_t total_tasks = 0;

task_t *new_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task = create_task(fptr, priority, period);
    if (task == NULL) {
        goto fail;
    }

    int ret = register_task(task, period);
    if (ret != 0) {
        goto fail2;
    }

    total_tasks += 1;

    return get_task_t(task);

fail2:
    free(task->stack_limit);
    kfree(task);
fail:
    panic_print("Could not allocate task with function pointer 0x%x", fptr);
}

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task;
    uint32_t *memory;
    static uint32_t pid_source = 1;
    task = (task_ctrl *) kmalloc(sizeof(task_ctrl));
    if (task == NULL) {
        return NULL;
    }

    memory = (uint32_t *) malloc(STKSIZE*4);
    if (memory == NULL) {
        kfree(task);
        return NULL;
    }

    task->stack_limit       = memory;
    task->stack_base        = memory + STKSIZE;
    task->stack_top         = memory + STKSIZE;
    task->fptr              = fptr;
    task->priority          = priority;
    task->running           = 0;
    task->abort             = 0;

    task->period            = period;
    task->ticks_until_wake  = period;
    task->pid               = pid_source++;

    list_init(&task->runnable_task_list);
    list_init(&task->periodic_task_list);
    list_init(&task->free_task_list);

    generic_task_setup(get_task_t(task));

    return task;
}

static int register_task(task_ctrl *task, int periodic) {
    /* When task switching, we cannot safely modify the task lists
     * ourselves, instead we must ask the OS to do so for us. */
    if (task_switching) {
        SVC_ARG2(SVC_REGISTER_TASK, task, periodic);
    }
    else {
        svc_register_task(task, periodic);
    }

    return 0;
}

void svc_register_task(task_ctrl *task, int periodic) {
    insert_task(runnable_task_list, task);

    if (periodic) {
        insert_task(periodic_task_list, task);
    }
}
