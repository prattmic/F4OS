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
       An illegal instruction in unprivileged mode
       results in a hard fault. */
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
       An illegal instruction in unprivileged mode
       results in a hard fault. */
    mrs     r1, control
    orr     r1, r1, #1
    msr     control, r1
    dsb
    isb
    bx      r0

/* Changes from unprivileged to privileged mode.
   This needs to be in .kernel so only the kernel can call it. */
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

/* Restores full context from PSP, including what the exeception handler would normally do. */
.thumb_func
.section    .kernel
.global     restore_full_context
.type       restore_full_context, %function
restore_full_context:
    /* Disable psp */
    mrs     r0, control
    bic     r0, r0, #2
    msr     control, r0
    dsb
    isb
    
    mrs     r0, psp
    /*and     r0, r0, #0
    add     r0, r0, sp*/
    ldmfd   r0!, {r4-r11}   /* Writes multiple registers and writes the final address back to Rn */
    add     r0, r0, #100    /* 25 words */
    msr     psp, r0

    sub     r0, r0, #72     /* Skip 18 words for FPU registers */
    /*vldm    r0, {s16-s31} */   /* Restore FPU registers */ /* Currently space is reserved for these, but they aren't saved */

    ldr     r1, [r0]        /* load xpsr */
    msr     xpsr_nzcvq, r1        /* Restore the PSR */
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

    ldr     r0, [sp, #-76]      /* load in old pc value */
    orr     r0, r0, #1          /* bit[0] must be set to 1 for tumb mode */
    str     r0, [sp, #-76]

    ldr     r0, [sp, #-100]
    ldr     pc, [sp, #-76]      /* load in pc value */

    /* Program is now in restored task */

/* Creates a fake context for the processor to restore */
.thumb_func
.section    .kernel
.global     create_context
.type       create_context, %function
create_context:
    mrs     r4, psp
    mov     r5, #0
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    stmdb   r4!, {r5}
    ldr.w   r5, =0x01000000 /* Thumb state bit must be set */
    stmdb   r4!, {r5}   /* xPSR */
    mov     r5, #0
    stmdb   r4!, {r0}   /* PC */
    stmdb   r4!, {r1}   /* LR */
    stmdb   r4!, {r5}   /* R12 */
    stmdb   r4!, {r5}   /* R3 */
    stmdb   r4!, {r5}   /* R2 */
    stmdb   r4!, {r5}   /* R1 */
    stmdb   r4!, {r5}   /* R0 */
    stmdb   r4!, {r5}   /* R11 */
    stmdb   r4!, {r5}   /* R10 */
    stmdb   r4!, {r5}   /* R9 */
    stmdb   r4!, {r5}   /* R8 */
    stmdb   r4!, {r3}   /* R7 - Frame Pointer*/
    stmdb   r4!, {r5}   /* R6 */
    stmdb   r4!, {r5}   /* R5 */
    stmdb   r4!, {r5}   /* R4 */
    msr     psp, r4
    bx      lr
