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
