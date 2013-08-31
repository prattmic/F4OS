#ifndef KERNEL_POWER_H_INCLUDED
#define KERNEL_POWER_H_INCLUDED

/**
 * Place the core in the lowest power state that still responds to interrupts.
 *
 * @returns 0 on wakeup from low power state,
 *          else if low power state not supported.
 */
int arch_wait_for_interrupt(void);

#endif
