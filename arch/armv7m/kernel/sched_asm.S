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

.syntax unified
.thumb

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16

/* Changes from unprivileged to privileged mode. */
.thumb_func
.section    .kernel
.global     raise_privilege
.type       raise_privilege, %function
raise_privilege:
    mrs     r0, control
    bic     r0, r0, #1
    msr     control, r0
    dsb
    isb
    bx      lr

/* Enables the PSP for user mode processes. */
.thumb_func
.section    .kernel
.global     enable_psp
.type       enable_psp, %function
enable_psp:
    mrs     r1, control
    orr     r1, r1, #2
    msr     control, r1
    dsb
    isb
    bx      lr

/* Switchs from PSP to MSP */
.thumb_func
.section    .kernel
.global     disable_psp
.type       disable_psp, %function
disable_psp:
    mrs     r0, control
    bic     r0, r0, #2
    msr     control, r0
    dsb
    isb
    bx      lr

/* Saves additional context not
 * saved by hardware on exception
 * entry. */
.thumb_func
.section    .kernel
.global     save_context
.type       save_context, %function
save_context:
    mrs     r0, psp
    stmfd   r0!, {r4-r11}   /* Saves multiple registers and writes the final address back to Rn */
#ifdef CONFIG_HAVE_FPU
    vstmdb  r0!, {s16-s31}  /* Save FPU registers */
#endif
    msr     psp, r0
    bx      lr

/* Restores part of the context from PSP, exception handler does the rest. */
.thumb_func
.section    .kernel
.global     restore_context
.type       restore_context, %function
restore_context:
    mrs     r0, psp
#ifdef CONFIG_HAVE_FPU
    vldmia  r0!, {s16-s31}  /* Restore FPU registers */
#endif
    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */
    msr     psp, r0
    bx      lr
