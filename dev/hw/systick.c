#include <stdint.h>
#include <dev/cortex_m.h>
#include <dev/hw/systick.h>

void init_systick(void) {
    *SYSTICK_RELOAD = CONFIG_SYS_CLOCK / CONFIG_SYSTICK_FREQ;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTL = 0x00000007;

    /* Set PendSV to lowest priority */
    *NVIC_IPR14 = 0xFF;
}

