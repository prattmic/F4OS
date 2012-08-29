#include <stddef.h>
#include <stdint.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include "sched_internals.h"

void systick_handler(void) __attribute__((section(".kernel"), naked));
void tim2_handler(void) __attribute__((section(".kernel")));
void pendsv_handler(void) __attribute__((section(".kernel"), naked));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

void systick_handler(void) {
    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;

    __asm__("bx lr");
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

void pendsv_handler(void){
    uint32_t *psp_addr;

    __asm__("push {lr}");
    psp_addr = save_context();

    curr_task->task->stack_top = psp_addr;

    switch_task();
    
    __asm__("pop {lr}\n"
            "b   restore_context\n");     /* Won't return */
}

void svc_handler(uint32_t *svc_args) {
    uint32_t svc_number;
    uint32_t return_address;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0] */
    svc_number = ((char *)svc_args[6])[-2];

    switch (svc_number) {
        case SVC_RAISE_PRIV: {
            /* Raise Privilege, but only if request came from the kernel */
            /* DEPRECATED: All code executed until _svc returns is privileged,
             * so raising privileges shouldn't ever be necessary. */
            return_address = svc_args[6];

            if (return_address >= (uint32_t) &_skernel && return_address < (uint32_t) &_ekernel) {
                raise_privilege();
            }
            else {
                panic_print("User code requested raised permissions.");
            }
            break;
        }
        case SVC_YIELD: {
            /* Set PendSV to yield a task */
            *SCB_ICSR |= SCB_ICSR_PENDSVSET;
            break;
        }
        case SVC_END_TASK: {
            register uint32_t lr_save asm("r9");
            __asm__("mov %[lr_save], lr"
                    :[lr_save] "=r" (lr_save)
                    ::"memory");

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

            enable_psp(curr_task->task->stack_top);

            __asm__("mov sp, %[ghetto]\n"
                    "mov lr, %[lr_save]\n"
                    "b  restore_context\n"    /* Won't return */
                    ::[ghetto] "r" (ghetto_sp_save), [lr_save] "r" (lr_save):);
            break;
        }
        case SVC_END_PERIODIC_TASK: {
            register uint32_t lr_save asm("r9");
            __asm__("mov %[lr_save], lr"
                    :[lr_save] "=r" (lr_save)
                    ::"memory");

            remove_task(&task_list, curr_task);

            curr_task->task->running = 0;
            /* Reset stack */
            curr_task->task->stack_top = curr_task->task->stack_base;

            switch_task();
            
            enable_psp(curr_task->task->stack_top);

            __asm__("mov sp, %[ghetto]\n"
                    "mov lr, %[lr_save]\n"
                    "b  restore_context\n"    /* Won't return */
                    ::[ghetto] "r" (ghetto_sp_save), [lr_save] "r" (lr_save):);
        }
        default:
            break;
    }
}
