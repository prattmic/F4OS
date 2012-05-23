#include "types.h"
#include "registers.h"
#include "systick.h"

#define SYSTEM_12_15_PRI (volatile uint32_t *)0xe000ed20

/* This file contains SysTick initialization function and SysTick interrupt handler */
void systick_init(void){
    *SYSTICK_CTL = 0x00000004;
    *SYSTICK_RELOAD = 0x40000;
    *SYSTICK_CTL |= 0x00000002;
    *SYSTICK_CTL |= 0x00000001;
    *SYSTEM_12_15_PRI |= (13 << 24); 
    asm("   CPSIE i\n \
            CPSIE f");
}

void systick_handler(void){
    *LED_ODR ^= (1<<12);
}
