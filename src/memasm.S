/* mem.S: assembler memory operations. */

.syntax unified
.thumb 

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16

/* Changes from unprivileged to privileged mode.
   This needs to be in .text so it has permissions to finish running itself */
.thumb_func
.section    .text
.global     user_mode
.type       user_mode, %function
user_mode:
    /* Enter unprivileged mode 
     * An illegal instruction in unprivileged mode
     * results in a hard fault. */
    mrs     r0, control
    orr     r0, r0, #1
    msr     control, r0
    dsb
    isb
    bx      lr

/* Switches to user mode and branches to R0 */
.thumb_func
.section    .text
.global     user_mode_branch
.type       user_mode_branch, %function
user_mode_branch:
    /* Enter unprivileged mode 
     * An illegal instruction in unprivileged mode
     * results in a hard fault. */
    mrs     r1, control
    orr     r1, r1, #1
    msr     control, r1
    dsb
    isb
    bx      r0

/* Changes from unprivileged to privileged mode.
 * This needs to be in .kernel so only the kernel can call it. */
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

/* Sets and enables the PSP for user mode processes 
 * The stack address is given as an argument and is
 * at r0. */
.thumb_func
.section    .kernel
.global     enable_psp
.type       enable_psp, %function
enable_psp:
    msr     psp, r0
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

/* Saves context */
.thumb_func
.section    .kernel
.global     save_context
.type       save_context, %function
save_context:
    mrs     r0, psp
    stmfd   r0!, {r4-r11}   /* Saves multiple registers and writes the final address back to Rn */
    msr     psp, r0
    bx      lr

/* Restores part of the context from PSP, exeception handler does the rest. */
.thumb_func
.section    .kernel
.global     restore_context
.type       restore_context, %function
restore_context:
    mrs     r0, psp
    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */
    msr     psp, r0
    bx      lr

/* Saves full context to PSP, including what the exeception handler would normally do. */
/* For save_full_context, see context.c:swap_task() */

/* Restores full context from PSP, including what the exeception handler would normally do. */
.thumb_func
.section    .kernel
.global     restore_full_context
.type       restore_full_context, %function
restore_full_context:
    /* Disable interrupts */
    cpsid   i

    /* Disable psp */
    mrs     r0, control
    bic     r0, r0, #2
    msr     control, r0
    dsb
    isb
    
    mrs     r0, psp

    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */
    add     r0, r0, #108    /* 25 words */
    msr     psp, r0

    sub     r0, r0, #72     /* Skip 18 words for FPU registers */
    /*vldm    r0, {s16-s31} */   /* Restore FPU registers */ /* Currently space is reserved for these, but they aren't saved */

    sub     r0, r0, #8
    ldr     r1, [r0]        /* load xpsr */
    msr     apsr_nzcvq, r1  /* Restore the PSR */
    msr     iepsr, r1       /* Restore the PSR */
    sub     r0, r0, #4      /* r0 now contains the PC */
    ldr     lr, [r0, #-4]
    ldr     r12, [r0, #-8]
    ldr     r3, [r0, #-12]
    ldr     r2, [r0, #-16]
    ldr     r1, [r0, #-20]

    /* Switch to psp */
    mrs     r0, control
    orr     r0, r0, #2
    msr     control, r0
    dsb
    isb

    ldr     r0, [sp, #-84]      /* load in old pc value */
    orr     r0, r0, #1          /* bit[0] must be set to 1 for thumb mode */
    str     r0, [sp, #-84]

    ldr     r0, [sp, #-108]

    /* Enable interrupts */
    cpsie   i

    ldr     pc, [sp, #-84]      /* load in pc value */

    /* Program is now in restored task */
