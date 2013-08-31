#include <arch/system.h>
#include <time.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched.h"

/* System tick interrupt handler */
void systick_handler(void) {
    system_ticks++;

    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

/* PendSV interrupt handler */
void pendsv_handler(void){
    /* Update periodic tasks */
    rtos_tick();

    get_task_ctrl(curr_task)->stack_top = PSP();

    switch_task(NULL);
}

uint32_t *get_user_stack_pointer(void) {
    return PSP();
}

void set_user_stack_pointer(uint32_t *stack_addr) {
    SET_PSP(stack_addr);
}

void sched_svc_yield(void) {
    get_task_ctrl(curr_task)->stack_top = PSP();
    switch_task(NULL);
}

void create_context(task_ctrl* task, void (*lptr)(void)) {
    asm volatile("stmdb   %[stack]!, {%[zero]}  /* Empty */                     \n\
                  stmdb   %[stack]!, {%[zero]}  /* FPSCR */                     \n\
                  stmdb   %[stack]!, {%[zero]}  /* S15 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S14 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S13 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S12 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S11 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S10 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S9 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S8 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S7 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S6 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S5 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S4 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S3 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S2 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S1 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S0 */                        \n\
                  stmdb   %[stack]!, {%[psr]}   /* xPSR */                      \n\
                  stmdb   %[stack]!, {%[pc]}    /* PC */                        \n\
                  stmdb   %[stack]!, {%[lr]}    /* LR */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R12 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* R3 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R2 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R1 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R0 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R11 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* R10 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* R9 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R8 */                        \n\
                  stmdb   %[stack]!, {%[frame]} /* R7 - Frame Pointer*/         \n\
                  stmdb   %[stack]!, {%[zero]}  /* R6 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R5 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* R4 */                        \n\
                  stmdb   %[stack]!, {%[zero]}  /* S31 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S30 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S29 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S28 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S27 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S26 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S25 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S24 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S23 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S22 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S21 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S20 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S19 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S18 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S17 */                       \n\
                  stmdb   %[stack]!, {%[zero]}  /* S16 */"
                  /* Output */
                  :[stack] "+r" (task->stack_top)
                  /* Input */
                  :[pc] "r" (task->fptr), [lr] "r" (lptr), [frame] "r" (task->stack_limit),
                   [zero] "r" (0), [psr] "r" (0x01000000) /* Set the Thumb bit */
                  /* Clobber */
                  :);
}
