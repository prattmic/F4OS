#include "types.h"
#include "registers.h"
#include "systick.h"


/* This file contains SysTick initialization function and SysTick interrupt handler */
void systick_init(void){
    *SYSTICK_RELOAD = 0x40000;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTL = 0x00000007;

    /* Enable LED for handler */
    *RCC_AHB1ENR |= (1 << 3);
    *GPIOD_MODER |= (1 << (12 * 2));
}

void systick_handler(void){
    *LED_ODR ^= (1<<12);
}
