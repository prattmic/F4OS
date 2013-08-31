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
    vstmdb  r0!, {s16-s31}  /* Save FPU registers */
    msr     psp, r0
    bx      lr

/* Restores part of the context from PSP, exception handler does the rest. */
.thumb_func
.section    .kernel
.global     restore_context
.type       restore_context, %function
restore_context:
    mrs     r0, psp
    vldmia  r0!, {s16-s31}  /* Restore FPU registers */
    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */
    msr     psp, r0
    b       enable_psp      /* User process uses the PSP */

/* For save_full_context, see f3f5b61 kernel/sched/sched_swap.c */

/* Restores full context from PSP, including what the exception handler would normally do. */
.thumb_func
.section    .kernel
.global     restore_full_context
.type       restore_full_context, %function
restore_full_context:
    bl      enable_psp      /* User process uses the PSP */

    mrs     r0, psp

    vldmia  r0!, {s16-s31}  /* Restore FPU registers */
    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */

    add     r0, r0, #12     /* Skip r0,r1,r2 for now */

    ldmia   r0!, {r3,r12,lr}

    add     r0, r0, #4      /* Skip PC for now */

    ldr     r1, [r0]        /* Load xPSR */
    msr     apsr_nzcvq, r1  /* Restore the xPSR */
    msr     iepsr, r1       /* Restore the xPSR */
    add     r0, r0, #4      /* Increment for xPSR */

    vldmia  r0!, {s0-s15}

    ldr     r1, [r0]        /* Load FPSCR */
    vmsr    fpscr, r1
    add     r0, r0, #4      /* Increment for FPSCR */

    ldr     r1, [r0, #-76]  /* Load in old PC value */

    orr     r1, r1, #1      /* bit[0] of PC must be set to 1 for thumb mode */
    str     r1, [r0, #4]    /* Stack PC */

    ldr     r1, [r0, #-100] /* Load r0 value */
    str     r1, [r0]        /* Stack r0 */

    ldr     r2, [r0, #-92]  /* Restore r2 */
    ldr     r1, [r0, #-96]  /* Restore r1 */

    msr     psp, r0

    pop     {r0, pc}

    /* Program is now in restored task */
