#include "types.h"
#include "registers.h"
#include "mem.h"
#include "context.h"
#include "task.h"
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
    uint32_t *psp_addr;

    /* Blink an LED, for the LOLs */
    *LED_ODR ^= (1<<12);

    __asm__("push {lr}");
    psp_addr = save_context();
    __asm__("pop {lr}");
    k_currentTask->task->stack_top = psp_addr;

    __asm__("push {lr}");
    switch_task();
    __asm__("pop {lr}");
    
    __asm__("push {lr}");
    restore_context();
    __asm__("pop {lr} \n"
            "bx lr\n");
}
