#include <stdint.h>
#include <arch/system.h>
#include <dev/hw/systick.h>

void init_systick(void) {
    *SYSTICK_RELOAD = CONFIG_SYS_CLOCK / CONFIG_SYSTICK_FREQ;
    *SYSTICK_VAL = 0;
    *SYSTICK_CTL = 0x00000007;

    /* Set PendSV and SVC to lowest priority.
     * This means that both will be deferred
     * until all other exceptions have executed.
     * Additionally, PendSV will not interrupt
     * an SVC. */
    *NVIC_IPR(11) = 0xFF;
    *NVIC_IPR(14) = 0xFF;
}

