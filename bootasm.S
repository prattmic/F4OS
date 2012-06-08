.syntax unified
.thumb 

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16

/* Vector table */
.section    .isr_vector,"a"
.word   _estack             /* stack top address */
.word   _reset              /* 1 Reset */
.word   hang                /* 2 NMI */
.word   hardfault_handler   /* 3 HardFault */
.word   memmanage_handler   /* 4 MemManage */
.word   busfault_handler    /* 5 BusFault */
.word   usagefault_handler  /* 6 UsageFault */
.word   hang                /* 7 RESERVED */
.word   hang                /* 8 RESERVED */
.word   hang                /* 9 RESERVED*/
.word   hang                /* 10 RESERVED */
.word   _svc                /* 11 SVCall */
.word   hang                /* 12 Debug Monitor */
.word   hang                /* 13 RESERVED */
.word   hang                /* 14 PendSV */
.word   systick_handler     /* 15 SysTick */
.word   hang                /* 16 External Interrupt(0) */
.word   hang                /* 17 External Interrupt(1) */
.word   hang                /* 18 External Interrupt(2) */
.word   hang                /* 19 ...   */

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
/* Copy the data segment initializers from flash to CCM RAM */  
    movs    r1, #0
    b       LoopCopyDataInit

CopyDataInit:
    ldr     r3, =_sidata
    ldr     r3, [r3, r1]
    str     r3, [r0, r1]
    adds    r1, r1, #4
    
LoopCopyDataInit:
    ldr     r0, =_sdata
    ldr     r3, =_edata
    adds    r2, r0, r1
    cmp     r2, r3
    bcc     CopyDataInit
    ldr     r2, =_sbss
    b       LoopFillZerobss
/* Zero fill the bss segment. */  
FillZerobss:
    movs    r3, #0
    str     r3, [r2], #4
    
LoopFillZerobss:
    ldr     r3, = _ebss
    cmp     r2, r3
    bcc     FillZerobss

    b       _start

.thumb_func
.global     _svc
.type       _svc, %function
_svc:
    mrs     r0, psp
    b       svc_handler
    bx      lr
