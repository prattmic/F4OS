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
.word   pendsv_handler      /* 14 PendSV */
.word   systick_handler     /* 15 SysTick */
/* NVIC */
.word   hang                /* 0 ... */
.word   hang                /* 1 ... */
.word   hang                /* 2 ... */
.word   hang                /* 3 ... */
.word   hang                /* 4 ... */
.word   hang                /* 5 ... */
.word   hang                /* 6 ... */
.word   hang                /* 7 ... */
.word   hang                /* 8 ... */
.word   hang                /* 9 ... */
.word   hang                /* 10 ... */
.word   hang                /* 11 ... */
.word   hang                /* 12 ... */
.word   hang                /* 13 ... */
.word   hang                /* 14 ... */
.word   hang                /* 15 ... */
.word   hang                /* 16 ... */
.word   hang                /* 17 ... */
.word   hang                /* 18 ... */
.word   hang                /* 19 ... */
.word   hang                /* 20 ... */
.word   hang                /* 21 ... */
.word   hang                /* 22 ... */
.word   hang                /* 23 ... */
.word   hang                /* 24 ... */
.word   hang                /* 25 ... */
.word   hang                /* 26 ... */
.word   hang                /* 27 ... */
.word   tim2_handler        /* 28 TIM2 Global Interrupt */
.word   hang                /* 29 ... */
.word   hang                /* 30 ... */
.word   hang                /* 31 ... */
.word   hang                /* 32 ... */
.word   hang                /* 33 ... */
.word   hang                /* 34 ... */
.word   hang                /* 35 ... */
.word   hang                /* 36 ... */
.word   hang                /* 37 USART1 Global Interrupt */
.word   hang                /* 38 ... */

/* Bootloader */
.section    .kernel, "ax"

.thumb_func
.global     _start
_start:
    /* main should never return */
    bl      os_start
    b       hang

/* Exception Handlers */
.thumb_func
.global     panic
.type       panic, %function
panic:
    /* Disable interrupts */
    cpsid   i
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

    b       _start

.thumb_func
.global     _svc
.type       _svc, %function
_svc:
    mrs     r0, psp
    b       svc_handler
    bx      lr
