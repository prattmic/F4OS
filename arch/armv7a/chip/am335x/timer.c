/*
 * Copyright (C) 2014 F4OS Authors
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

#include <libfdt.h>
#include <stdio.h>
#include <dev/clocks.h>
#include <dev/fdtparse.h>
#include <dev/raw_mem.h>
#include <dev/hw/systick.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "clocks.h"
#include "interrupts.h"
#include "timer.h"

#define AM335X_DMTIMER_1MS_COMPAT   "ti,am33xx-dmtimer-1ms"

#define TIMER_FREQ  (CLK_M_OSC)
#define TIMER_CNT_PER_SYSTICK   (TIMER_FREQ/CONFIG_SYSTICK_FREQ)

/* Registers passed into handler */
void systick_handler(void *data) {
    struct am335x_dmtimer_1ms *regs = data;

    /* Acknowledge interrupt */
    raw_mem_set_bits(&regs->tisr, AM335X_DMTIMER_TISR_OVF_IT_FLAG);

    /* Perform OS system tick */
    sched_system_tick();
}

void init_systick(void) {
    const void *fdt = fdtparse_get_blob();
    struct am335x_dmtimer_1ms *regs;
    int offset, len;
    fdt32_t *cell;
    const struct fdt_property *interrupts;
    uint32_t interrupt_num;
    uint32_t tldr_val;

    /* HACK: Simply use the first DMTimer 1ms */
    offset = fdt_node_offset_by_compatible(fdt, -1, AM335X_DMTIMER_1MS_COMPAT);
    if (offset < 0) {
        panic_print("DMTimer 1ms not found");
    }

    regs = fdtparse_get_addr32(fdt, offset, "regs");
    if (!regs) {
        panic_print("DMTimer 1ms registers not found");
    }

    /* Get interrupt number */
    interrupts = fdt_get_property(fdt, offset, "interrupts", &len);
    /* Make sure there is room for one interrupt */
    if (len < 0 || len < sizeof(fdt32_t)) {
        panic_print("Unable to get DMTimer 1ms interrupt number");
    }

    cell = (fdt32_t *) interrupts->data;
    /* There is a single interrupt */
    interrupt_num = fdt32_to_cpu(cell[0]);

    /* Select master oscillator as clock */
    if (clocks_set_param(fdt, offset, "ti,clock-select",
                         AM335X_DMTIMER_1MS_CLK_M_OSC)) {
        panic_print("Unable to set DMTimer 1ms clock source");
    }

    /* Enable module clock */
    if (clocks_enable(fdt, offset, "clocks")) {
        panic_print("Unable to enable DMTimer 1ms module clock");
    }

    if (!TIMER_CNT_PER_SYSTICK) {
        panic_print("Unable to achieve system tick frequency");
    }

    /*
     * We want TIMER_CNT_PER_SYSTICK timer counts between the
     * load value and overflow.
     */
    tldr_val = (1LL << 32) - TIMER_CNT_PER_SYSTICK;
    raw_mem_write(&regs->tldr, tldr_val);

    /* Force reload of counter */
    raw_mem_write(&regs->ttgr, 1);

    /* Enable overflow interrupt */
    raw_mem_set_bits(&regs->tier, AM335X_DMTIMER_TIER_OVF_IT_EN);

    /* Register and enable interrupt.  Pass registers to handler */
    if (am335x_interrupt_register(fdt, offset, interrupt_num,
                                  systick_handler, regs)) {
        panic_print("Unable to register DMTimer 1ms interrupt");
    }

    if (am335x_interrupt_enable(fdt, offset, interrupt_num)) {
        panic_print("Unable to enable DMTimer 1ms interrupt");
    }

    /* Start timer free running */
    raw_mem_write(&regs->tclr, AM335X_DMTIMER_TCLR_ST |
                               AM335X_DMTIMER_TCLR_AR);
}
