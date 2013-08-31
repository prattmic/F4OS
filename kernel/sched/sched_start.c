#include <dev/hw/systick.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

volatile uint8_t task_switching = 0;
task_t * volatile curr_task;

static void start_task_switching(void) __attribute__((section(".kernel")));

void start_sched(void) {
    /* Set up initial tasks */
    new_task(&kernel_task, 10, 4);
    new_task(&sleep_task, 0, 0);

    /* Setup boot tasks specified by end user. */
    main();

    init_systick();
    start_task_switching();
}

static void start_task_switching(void) {
    if (list_empty(&runnable_task_list)) {
        panic_print("No tasks to run!");
    }

    struct list *element = runnable_task_list.next;

    task_ctrl *task = list_entry(element, task_ctrl, runnable_task_list);

    curr_task = get_task_t(task);

    //mpu_stack_set(task->stack_base);

    task_switching = 1;
    task->running = 1;

    create_context(task, &end_task);

    set_user_stack_pointer(task->stack_top);
    restore_full_context();
}

