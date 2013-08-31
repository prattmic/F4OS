#include <stdint.h>
#include <time.h>
#include <kernel/sched.h>

volatile uint32_t system_ticks = 0;

int usleep(uint32_t usecs) {
    /* Go ahead and save this in case it changes during out calculations */
    uint32_t sys_time = system_ticks;

    /* Tick period in usecs */
    uint32_t period = 1e6/CONFIG_SYSTICK_FREQ;

    uint32_t delta_ticks = (usecs + period - 1)/period;

    uint32_t end_time = sys_time + delta_ticks;

    /* Since system_ticks is only 32-bits, it will overflow rather quickly
     * (12 days @ 250us systick), so we should check for the overflow
     * condition */
    if (sys_time > end_time) {
        /* Wait for overflow to occur before continuing to main loop */
        while (system_ticks > end_time) {
            yield_if_possible();
        }
    }

    while (system_ticks < end_time) {
        yield_if_possible();
    }

    return 0;
}
