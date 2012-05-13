.syntax unified
.thumb 

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16

/* Vector table */
.section    .isr_vector,"a"
.word   _estack     /* stack top address */
.word   _reset      /* 1 Reset */
.word   hang        /* 2 NMI */
.word   hang        /* 3 HardFault */
.word   panic       /* 4 MemManage */
.word   hang        /* 5 BusFault */
.word   hang        /* 6 UsageFault */
.word   hang        /* 7 RESERVED */
.word   hang        /* 8 RESERVED */
.word   hang        /* 9 RESERVED*/
.word   hang        /* 10 RESERVED */
.word   hang        /* 11 SVCall */
.word   hang        /* 12 Debug Monitor */
.word   hang        /* 13 RESERVED */
.word   hang        /* 14 PendSV */
.word   hang        /* 15 SysTick */
.word   hang        /* 16 External Interrupt(0) */
.word   hang        /* 17 External Interrupt(1) */
.word   hang        /* 18 External Interrupt(2) */
.word   hang        /* 19 ...   */

/* Bootloader */
.section    .kernel, "ax"

.thumb_func
.global     _start
_start:
    bl      enable_float

    /* main should never return */
    bl      main
    b       hang

#define SCB_CPACR   0xE000ED88  /* Coprocessor Access Control Register - Enables FPU */
.thumb_func
enable_float:
    /* Enable the floating point unit */
    ldr.w   r0, =SCB_CPACR
    ldr     r1, [r0]
    orr     r1, r1, #(0xF << 20)
    str     r1, [r0]
    dsb     /* Wait for write to finish */
    isb
    bx      lr


/* Exception Handlers */
.thumb_func
.global     panic
.type       panic, %function
panic:
    bl      raise_privilege
    bl      toggle_led_delay
    b       panic 

.thumb_func
hang:
    b       .

.thumb_func
_reset:
    b _start
