#include <dev/registers.h>
#include <dev/hw/systick.h>

#include <kernel/sched.h>
#include "sched_internals.h"

uint8_t task_switching = 0;

static void start_task_switching(void) __attribute__((section(".kernel")));

void start_sched(void) {
    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));

    /* Set up initial tasks */
    new_task(&kernel_task, 1, 0);

    /* Setup boot tasks specified by end user. */
    main();

    init_systick();
    start_task_switching();
}

static void start_task_switching(void) {
    task_ctrl *task = task_list.head->task;

    curr_task = task_list.head;

    //mpu_stack_set(task->stack_base);

    task_switching = 1;
    task->running = 1;
    
    if (task->period) {
        create_context(task, &end_periodic_task);
    }
    else {
        create_context(task, &end_task);
    }

    enable_psp(task->stack_top);
    restore_full_context();
    __asm__("nop");
}

