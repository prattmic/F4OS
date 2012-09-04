#include <stddef.h>
#include <stdint.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include "sched_internals.h"

void systick_handler(void) __attribute__((section(".kernel"), naked));
void pendsv_handler(void) __attribute__((section(".kernel")));
void tim2_handler(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

void systick_handler(void) {
    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;

    __asm__("bx lr");
}

void pendsv_handler(void){
    curr_task->task->stack_top = PSP();

    switch_task();
}

void tim2_handler(void) {
    *TIM2_SR = 0;

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

void svc_handler(uint32_t *svc_args) {
    uint32_t svc_number;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0] */
    svc_number = ((char *)svc_args[6])[-2];

    switch (svc_number) {
        case SVC_YIELD: {
            curr_task->task->stack_top = PSP();

            switch_task();
            break;
        }
        case SVC_END_TASK: {
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

            remove_task(&task_list, curr_task);

            switch_task();

            node->next = NULL;
            break;
        }
        case SVC_END_PERIODIC_TASK: {
            remove_task(&task_list, curr_task);

            curr_task->task->running = 0;
            /* Reset stack */
            curr_task->task->stack_top = curr_task->task->stack_base;

            switch_task();
            break;
        }
        default:
            break;
    }
}
