/*
 * Copyright (C) 2013 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <arch/chip.h>
#include <arch/system.h>
#include <dev/hw/systick.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>

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

    /* Enable floating point extension, and thus state preservation */
    asm volatile ("mrs  r0, control \t\n"
                  "orr  r0, %[fpca] \t\n"
                  "msr  control, r0 \t\n"
                  :: [fpca] "I" (CONTROL_FPCA)
                  : "r0");
#endif
}

void arch_sched_start_system_tick(void) {
    /* The SysTick timer handles system ticks */
    init_systick();
}
