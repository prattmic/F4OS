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

void svc_handler(uint32_t *registers) {
    uint32_t svc_number;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument and return value (r0) is registers[0] */
    svc_number = ((char *)registers[6])[-2];

    switch (svc_number) {
        case SVC_YIELD: {
            curr_task->task->stack_top = PSP();

            switch_task();
            break;
        }
        case SVC_END_TASK: {
            svc_end_task();
            break;
        }
        default:
            break;
    }
}
