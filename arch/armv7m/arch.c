#include <arch/chip.h>
#include <arch/system.h>

/* Set up universal Cortex M perihperals/system settings */
void init_arch(void) {
    /* Setup chip clocks */
    init_clock();

    /* Send event on pending interrupt */
    *SCB_SCR |= SCB_SCR_SEVONPEND;

    /* Enable Bus and Usage Faults */
    *SCB_SHCSR |= SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USEFAULTENA;

#ifdef CONFIG_HAVE_FPU
    /* Enable the FPU */
    *SCB_CPACR |= SCB_CPACR_CP10_FULL | SCB_CPACR_CP11_FULL;

    /* Enable floating point state preservation */
    *FPU_CCR |= FPU_CCR_ASPEN;
#endif
}
