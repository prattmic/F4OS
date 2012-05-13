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
