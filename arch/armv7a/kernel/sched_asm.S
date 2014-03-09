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

#include <arch/system_regs.h>

.syntax unified

/* Get SP_usr and LR_usr
 *
 * @returns SP_usr in r0, LR_usr in r1
 */
get_user_sp_lr:
    /* Get current processor mode */
    mrs     r2, cpsr
    and     r2, r2, #CPSR_MODE

    /* Switch to system mode */
    cps     #CPSR_MODE_SYS

    /* Copy SP, LR */
    mov     r0, sp
    mov     r1, lr

    /* Switch back to original processor mode */
    mrs     r3, cpsr
    bic     r3, r3, #CPSR_MODE
    orr     r3, r3, r2
    msr     cpsr, r3

    bx      lr

.section    .kernel
.global     get_user_stack_pointer
.type       get_user_stack_pointer, %function
get_user_stack_pointer:
    /*
     * Get SP_usr and LR_usr.  Since SP_usr is returned in r0, we
     * can immediately return, since we have what we need.
     */
    push    {lr}
    bl      get_user_sp_lr
    pop     {lr}

    bx      lr

.section    .kernel
.global     set_user_stack_pointer
.type       set_user_stack_pointer, %function
set_user_stack_pointer:
    /* Get current processor mode */
    mrs     r1, cpsr
    and     r1, r1, #CPSR_MODE

    /* Switch to system mode */
    cps     #CPSR_MODE_SYS

    /* Copy SP */
    mov     sp, r0

    /* Switch back to original processor mode */
    mrs     r2, cpsr
    bic     r2, r2, #CPSR_MODE
    orr     r2, r2, r1
    msr     cpsr, r2

    bx      lr

/* Set LR_usr = r0 */
set_user_lr:
    /* Get current processor mode */
    mrs     r1, cpsr
    and     r1, r1, #CPSR_MODE

    /* Switch to system mode */
    cps     #CPSR_MODE_SYS

    /* Copy LR */
    mov     lr, r0

    /* Switch back to original processor mode */
    mrs     r2, cpsr
    bic     r2, r2, #CPSR_MODE
    orr     r2, r2, r1
    msr     cpsr, r2

    bx      lr

/*
 * Save the context of the previous mode on the stack.
 *
 * If the previous mode was user mode, then it will be saved on the
 * user stack, otherwise it will be stored on the local stack, as
 * context is only changed by the OS for user mode.  For other modes,
 * we are just saving state for exception handling.
 *
 * This is expected to be called in a privileged mode other than FIQ.
 * This function clobbers r0-r3.
 *
 * The context is saved on the stack like so:
 *
 *  ---------------
 *  |             | <- Original SP (e.g., 0x103C)
 *  | r15 (PC)    |
 *  | r12         |  | Stack grows down
 *  | r11         |  |
 *  | r10         |  V
 *  | r9          |
 *  | r8          |
 *  | r7          |
 *  | r6          |
 *  | r5          |
 *  | r4          |
 *  | r3          |
 *  | r2          |
 *  | r1          |
 *  | r0          |
 *  | SPSR (CPSR) | <- Final SP (e.g., 0x1000)
 *  ---------------
 *
 * @param   Other mode PC, on the stack (address to return from exception to)
 * @returns Saved registers, in r0 (struct stacked_registers in sched_asm.h)
 */
.section    .kernel
.global     save_context
.type       save_context, %function
save_context:
    /* Original PC already on the stack */

    /* Save r0-r12 */
    stmfd   sp!, {r0-r12}

    /* Save SPSR */
    mrs     r1, spsr
    stmfd   sp!, {r1}

    /*
     * Return pointer to registers, or pass to
     * copy_context_user, which will return pointer
     * to registers on user stack,
     */
    mov     r0, sp

    /* Don't copy to user stack before task switching starts */
    ldr     r2, =task_switching
    ldr     r2, [r2]
    cmp     r2, #0
    beq     skip_user_check

    and     r1, r1, #CPSR_MODE
    teq     r1, #CPSR_MODE_USR
    beq     copy_context_user

skip_user_check:
    bx      lr

/*
 * Copy context (as returned by save_context), onto the user
 * stack, updating the user stack pointer.
 *
 * Additionally, push the LR onto the user stack.
 *
 *  ---------------
 *  |             | <- Original SP_usr (e.g., 0x1040)
 *  | r15 (PC)    |
 *  | r12         |  | Stack grows down
 *  | r11         |  |
 *  | r10         |  V
 *  | r9          |
 *  | r8          |
 *  | r7          |
 *  | r6          |
 *  | r5          |
 *  | r4          |
 *  | r3          |
 *  | r2          |
 *  | r1          |
 *  | r0          |
 *  | SPSR (CPSR) |
 *  | LR          | <- Final SP_usr (e.g., 0x1000)
 *  ---------------
 *
 * @param   Pointer to context (as returned by save_context), in r0
 * @returns Pointer to context on user stack (skipping LR), in r0
 */
.section    .kernel
.global     copy_context_user
.type       copy_context_user, %function
copy_context_user:
    /* Get current processor mode */
    mrs     r1, cpsr
    and     r1, r1, #CPSR_MODE

    /* Switch to system mode */
    cps     #CPSR_MODE_SYS

    /* Copy 15 words */
    mov     r3, #4*14

loop:
    ldr     r2, [r0, r3]
    push    {r2}
    subs    r3, r3, #4
    bpl     loop

    /* Return context pointer without LR */
    mov     r0, sp

    /* Save LR */
    push    {lr}

    /* Switch back to original processor mode */
    mrs     r2, cpsr
    bic     r2, r2, #CPSR_MODE
    orr     r2, r2, r1
    msr     cpsr, r2

    bx      lr

/*
 * Restore the context of the mode returned to from the exception.
 *
 * If restoring to user mode, registers are restored from the user
 * mode stack, otherwise from the exception stack.
 *
 * This is expected to be called in a privileged mode other than FIQ.
 *
 * The context is restored from the format described in save_context.
 *
 * This function clobbers r0-r12, SPSR.
 *
 * @returns (On stack) LR to return with
 */
.section    .kernel
.global     restore_context
.type       restore_context, %function
restore_context:
    /* Restore SPSR */
    pop     {r0}
    msr     spsr, r0

    /*
     * If returning to user mode, get registers from
     * user stack instead, as we may have switched tasks
     */
    and     r0, r0, #CPSR_MODE
    teq     r0, #CPSR_MODE_USR
    beq     restore_context_usr

    /* Restore r0-r12 */
    ldmfd   sp!, {r0-r12}

    /* Leave PC on stack to be popped as interrupt LR */
    bx      lr

/* The user LR is additionally on the stack, restore it too */
restore_context_usr:
    /*
     * Discard context saved on stack.  This was the old user
     * context.  We may have switched to a new user task, in
     * which case this old context has already been saved on
     * its user stack.
     */
    add     sp, sp, #4*14

    push    {lr}

    /* r0 = sp_usr */
    bl      get_user_stack_pointer

    /* r4 = sp_usr */
    mov     r4, r0

    /* We will end up popping LR, SPSR, r0-r12, PC */
    add     r0, r4, #4*16
    bl      set_user_stack_pointer

    /* Pop LR, set it */
    ldmfd   r4!, {r0}
    bl      set_user_lr

    pop     {lr}

    /* Extract PC, place on stack to be popped as interrupt LR */
    ldr     r0, [r4, #4*14]
    push    {r0}

    /* Restore SPSR */
    ldmfd   r4!, {r0}
    msr     spsr, r0

    /* Restore r0-r12 */
    ldmfd   r4, {r0-r12}

    bx      lr
