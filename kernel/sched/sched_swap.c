#include <stdint.h>

#include <kernel/sched.h>
#include "sched_internals.h"

/* Makes node run immediately */
void swap_task(task_node *node) {
    register uint32_t *stack_top asm("r1");

    __asm__(/* If you aren't already on the psp, you are screwed, sorry */
            "str     r1, [sp, #-96] \r\n"      /* Go ahead and store r0, so I can use it */

            "mrs     r1, xpsr        \r\n"
            "orr     r1, r1, #0x1000000      \r\n"
            "str     r1, [sp, #-72]  \r\n"      /* Store xpsr quickly, don't want it to change */

            /* 18 words left for fp registers, which I don't save yet */

            "ldr     r1, =swap_complete  \r\n"
            "str     r1, [sp, #-76]  \r\n"      /* Save PC as the end of this function */

            "str     lr, [sp, #-80]  \r\n"
            "str     r12, [sp, #-84] \r\n"
            "str     r3, [sp, #-88]  \r\n"
            "str     r2, [sp, #-92]  \r\n"
            "str     r0, [sp, #-100]  \r\n"
           
            "sub     r1, sp, #100  \r\n"
            "stmfd   r1!, {r4-r11}  \r\n"   /* Saves multiple registers and writes the final address back to Rn */

            "msr     psp, r1  \r\n"
            );

    curr_task->task->stack_top = stack_top;

    curr_task = node;

    enable_psp(curr_task->task->stack_top);

    __asm__("b  restore_full_context\r\n");  /* Won't return */

    __asm__("swap_complete: \r\n"
            "nop");
}
