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

#ifndef ARCH_ARMV7A_CHIP_AM335X_INTERRUPTS_H_INCLUDED
#define ARCH_ARMV7A_CHIP_AM335X_INTERRUPTS_H_INCLUDED

#include <stdint.h>

enum interrupts {
    AM335X_IRQ_TINT0 = 66,
    AM335X_IRQ_TINT1_1MS,
    AM335X_IRQ_TINT2,
    AM335X_IRQ_TINT3,
    AM335X_IRQ_TINT4 = 92,
    AM335X_IRQ_TINT5,
    AM335X_IRQ_TINT6,
    AM335X_IRQ_TINT7,
    AM335X_IRQ_NUM = 128,
};

enum interrupt_type {
    AM335X_IRQ,
    AM335X_FIQ,
};

struct am335x_intc_bank {
    uint32_t itr;
    uint32_t mir;
    uint32_t mir_clear;
    uint32_t mir_set;
    uint32_t isr_set;
    uint32_t isr_clear;
    uint32_t pending_irq;
    uint32_t pending_fiq;
};

struct am335x_intc {
    uint32_t revision;
    uint32_t reserved1[3];
    uint32_t sysconfig;
    uint32_t sysstatus;
    uint32_t reserved2[10];
    uint32_t sir_irq;
    uint32_t sir_fiq;
    uint32_t control;
    uint32_t protection;
    uint32_t idle;
    uint32_t reserved3[3];
    uint32_t irq_priority;
    uint32_t fiq_priority;
    uint32_t threshold;
    uint32_t reserved4[5];
    struct am335x_intc_bank bank[3];
    uint32_t ilr[AM335X_IRQ_NUM];
};

#define AM335X_INTC_SYSCONFIG_AUTOIDLE      (1 << 0)    /* Auto-idle */

#define AM335X_INTC_SIR_IRQ_ACTIVE_MASK     (0x7ff)     /* Active IRQ */
#define AM335X_INTC_SIR_FIQ_ACTIVE_MASK     (0x7ff)     /* Active FIQ */

#define AM335X_INTC_CONTROL_NEWIRQAGR       (1 << 0)    /* Allow new IRQ generation */
#define AM335X_INTC_CONTROL_NEWFIQAGR       (1 << 1)    /* Allow new FIQ generation */

#define AM335X_INTC_ILR_FIQ                 (1)         /* Interrupt asserts FIQ */
#define AM335X_INTC_ILR_PRIORITY_SHIFT      (2)
#define AM335X_INTC_ILR_PRIORITY_MASK       (0xFC)

/**
 * Register interrupt handler
 *
 * For an interrupt controlled by the AM335x interrupt controller,
 * register interrupt handler to be called upon interrupt assertion.
 *
 * The passed data pointer will be passed into the interrupt handler,
 * and may be used to pass in a custom data structure.
 *
 * This function should clear the interrupt, if applicable.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @param func  void func(void*), function to call when interrupt asserted.
 * @param data  Pointer to pass into func on every call.
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_register(const void *fdt, int nodeoffset, uint8_t num,
                              void (*func)(void*), void *data);

/**
 * Unregister interrupt handler
 *
 * For an interrupt controlled by the AM335x interrupt controller,
 * unregister interrupt handler called upon interrupt assertion.
 *
 * In its place, and "unhandled interrupt" handler will be called.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_unregister(const void *fdt, int nodeoffset, uint8_t num);

/**
 * Set interrupt priority
 *
 * Set priority for an interrupt controlled by the AM335x
 * interrupt controller.
 *
 * 0x0 is the highest priority, and 0x7F is the lowest priority.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @param priority  Priority value to assign
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_priority(const void *fdt, int nodeoffset, uint8_t num,
                              uint8_t priority);

/**
 * Set interrupt as FIQ/IRQ
 *
 * For an interrupt controlled by the AM335x interrupt controller,
 * assign the interrupt to assert the FIQ or IRQ ARM interrupt.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @param fiq   1 to assert FIQ, 0 to assert IRQ
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_fiq(const void *fdt, int nodeoffset, uint8_t num,
                         uint8_t fiq);

/**
 * Enable interrupt
 *
 * For an interrupt controlled by the AM335x interrupt controller,
 * enable interrupt assertion.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_enable(const void *fdt, int nodeoffset, uint8_t num);

/**
 * Disable interrupt
 *
 * For an interrupt controlled by the AM335x interrupt controller,
 * disable interrupt assertion.
 *
 * @param fdt   Pointer to FDT blob
 * @param nodeoffset    Offset of FDT node associated with interrupt.
 *  This is used to find the interrupt-parent.
 * @param num   Interrupt number to modify
 * @returns 0 on success, negative on failure
 */
int am335x_interrupt_disable(const void *fdt, int nodeoffset, uint8_t num);

#endif
