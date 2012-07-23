#include "types.h"
#include "registers.h"
#include "task.h"
#include "context.h"
#include "mem.h"
#include "systick.h"


/* This file contains SysTick initialization function and SysTick interrupt handler */
void systick_init(void){
    *SYSTICK_RELOAD = 0x40000;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTL = 0x00000007;

    /* Set PendSV to lowest priority */
    *NVIC_IPR14 = 0xFF;

    /* Enable LED for handler */
    *RCC_AHB1ENR |= (1 << 3);
    *GPIOD_MODER |= (1 << (12 * 2));
}

void systick_handler(void) {
    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;

    __asm__("bx lr");
}
