/*
 * Copyright (C) 2013 F4OS Authors
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

/* Exception Handlers */
.syntax unified
.thumb

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16

.section    .kernel, "ax"

.thumb_func
.global     panic
.type       panic, %function
panic:
    /* Disable interrupts */
    cpsid   i
    bl      raise_privilege
#ifdef CONFIG_HAVE_LED
    bl      toggle_led_delay
#endif
    b       panic

.thumb_func
.global     disable_interrupts
.type       disable_interrupts, %function
disable_interrupts:
    cpsid   i
    bx      lr

.thumb_func
.global hang
hang:
    b       .

.thumb_func
.global _reset
_reset:
    /*
     * Switch to the PSP
     *
     * Reserve 512 bytes of stack space for the MSP,
     * which will be used in interrupt context.
     *
     * After task switching begins, only user stacks
     * will be on the PSP, and this initial kernel
     * stack will never be used again,
     */
    sub     r0, sp, #512
    msr     psp, r0
    bl      enable_psp

    /* Copy the data segment initializers from flash to CCM RAM */
    movs    r1, #0
    b       loop_copy_data

copy_data_init:
    ldr     r3, =_sidata
    ldr     r3, [r3, r1]
    str     r3, [r0, r1]
    adds    r1, r1, #4

loop_copy_data:
    ldr     r0, =_sdata
    ldr     r3, =_edata
    adds    r2, r0, r1
    cmp     r2, r3
    bcc     copy_data_init
    ldr     r2, =_sbss
    b       loop_zero_bss

/* Zero fill the bss segment. */
zero_bss:
    movs    r3, #0
    str     r3, [r2], #4

loop_zero_bss:
    ldr     r3, = _ebss
    cmp     r2, r3
    bcc     zero_bss

    /* To the OS! */
    b       os_start

.thumb_func
.global     _svc_asm
.type       _svc_asm, %function
_svc_asm:
    mrs     r0, psp

    push    {lr}
    push    {r0}
    bl      save_context
    pop     {r0}
    bl      svc_handler
    bl      restore_context
    pop     {lr}
    bx      lr

.thumb_func
.global _pendsv
_pendsv:
    push    {lr}
    bl      save_context
    bl      pendsv_handler
    bl      restore_context
    pop     {lr}
    bx      lr
