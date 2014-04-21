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

#ifndef ARCH_CHIP_H_INCLUDED
#define ARCH_CHIP_H_INCLUDED

/*
 * Perform core chip setup
 *
 * Called extremely early in boot.  This should perform core essential
 * chip setup, before anything else is run.  Most notably, this includes
 * core clock setup.
 */
void init_chip(void) __attribute__((section(".kernel")));

/*
 * Handle IRQ
 *
 * Called when an IRQ interrupt is raised.  Should determine the IRQ
 * source, and handle it appropriately.  System state is saved before
 * calling this function, and restored when it returns.
 */
void irq_handler(void) __attribute__((section(".kernel")));

/*
 * Handle FIQ
 *
 * Called when an FIQ interrupt is raised.  Should determine the FIQ
 * source, and handle it appropriately.  System state is saved before
 * calling this function, and restored when it returns.
 */
void fiq_handler(void) __attribute__((section(".kernel")));

/**
 * Enable chip system tick timer
 *
 * Enable system tick timer, as described by the docs for
 * arch_sched_start_system_tick() in include/kernel/sched_internals.h.
 */
void chip_sched_start_system_tick(void);

#endif
