/*
 * Copyright (C) 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stddef.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include <arch/system_regs.h>

/*
 * Create context in format defined by save_context
 */
void create_context(task_ctrl* task, void (*lptr)(void)) {
    /* AAPCS requires an 8-byte aligned SP */
    uintptr_t stack = (uintptr_t) task->stack_top;
    stack -= stack % 8;
    task->stack_top = (uint32_t *) stack;

    /* This runs in user mode, obviously */
    uint32_t cpsr = CPSR_MODE_USR;

    /* Set Thumb bit if address is a Thumb address */
    if (((uintptr_t) task->fptr) & 1) {
        cpsr |= CPSR_THUMB;
    }

    asm volatile("stmdb   %[stack]!, {%[pc]}    /* PC */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R12 */  \n\
                  stmdb   %[stack]!, {%[zero]}  /* R11 */  \n\
                  stmdb   %[stack]!, {%[zero]}  /* R10 */  \n\
                  stmdb   %[stack]!, {%[zero]}  /* R9 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R8 */   \n\
                  stmdb   %[stack]!, {%[frame]} /* R7 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R6 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R5 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R4 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R3 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R2 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R1 */   \n\
                  stmdb   %[stack]!, {%[zero]}  /* R0 */   \n\
                  stmdb   %[stack]!, {%[cpsr]}  /* CPSR */ \n\
                  stmdb   %[stack]!, {%[lr]}    /* LR */   \n"
                  /* Output */
                  :[stack] "+r" (task->stack_top)
                  /* Input */
                  :[pc] "r" (task->fptr), [lr] "r" (lptr),
                   [frame] "r" (task->stack_limit), [zero] "r" (0),
                   [cpsr] "r" (cpsr)
                  /* Clobber */
                  :);
}

/* Switch into user mode in perparation for task switching */
void arch_sched_start_bootstrap(void) {
    /*
     * Take the SP and LR along with us as we go into user mode,
     * to ensure we can return and continue execution as normal.
     */
    asm volatile("mov   r0, sp  \n\
                  mov   r1, lr  \n\
                  cps   %[mode] \n\
                  mov   lr, r1  \n\
                  mov   sp, r0  \n"
                 ::[mode] "I" (CPSR_MODE_USR)
                 : "r0", "r1");
}
