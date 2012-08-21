#include <stdint.h>
#include <dev/registers.h>
#include <dev/hw/systick.h>

void init_systick(void) {
    *SYSTICK_RELOAD = 0x40000;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTL = 0x00000007;

    /* Set PendSV to lowest priority */
    *NVIC_IPR14 = 0xFF;

    /* Enable LED for handler */
    *RCC_AHB1ENR |= (1 << 3);
    *GPIOD_MODER |= (1 << (12 * 2));
}

