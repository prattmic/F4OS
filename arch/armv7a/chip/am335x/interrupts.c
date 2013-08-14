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
#include <stdint.h>
#include <stdio.h>
#include <dev/fdtparse.h>
#include <dev/raw_mem.h>
#include <kernel/init.h>
#include <kernel/fault.h>
#include "memory_map.h"
#include "interrupts.h"

#define AM335X_INTC_COMPAT  "ti,am33xx-intc"

struct am335x_intc *primary_intc;

/* An unhandled interrupt does nothing */
static void unhandled_interrupt(void *data) {
    printk("OOPS: Unhandled interrupt\r\n");
    return;
}

static void (*interrupt_handlers[AM335X_IRQ_NUM])(void*) = {
    [0 ... (AM335X_IRQ_NUM-1)] = unhandled_interrupt,
};

static void *interrupt_handler_data[AM335X_IRQ_NUM];

void irq_handler(void) {
    int active_interrupt = raw_mem_read(&primary_intc->sir_irq) &
                            AM335X_INTC_SIR_IRQ_ACTIVE_MASK;

    void *data = interrupt_handler_data[active_interrupt];

    interrupt_handlers[active_interrupt](data);

    /* Allow new IRQ generation */
    raw_mem_set_bits(&primary_intc->control, AM335X_INTC_CONTROL_NEWIRQAGR);
}

void fiq_handler(void) {
    int active_interrupt = raw_mem_read(&primary_intc->sir_fiq) &
                            AM335X_INTC_SIR_FIQ_ACTIVE_MASK;

    void *data = interrupt_handler_data[active_interrupt];

    interrupt_handlers[active_interrupt](data);

    /* Allow new FIQ generation */
    raw_mem_set_bits(&primary_intc->control, AM335X_INTC_CONTROL_NEWFIQAGR);
}

/*
 * Get the register bank for the interrupt-parent of a node
 *
 * Look up the interrupt-parent, ensure driver compatibility, and get the
 * registers
 *
 * @param fdt   pointer to FDT blob
 * @param nodeoffset    Offset of node to find interrupt-parent of
 * @returns pointer to interrupt-parent regs, NULL on error
 */
static struct am335x_intc *interrupt_parent_regs(const void *fdt,
                                                 int nodeoffset) {
    int intc;
    struct am335x_intc *regs;

    intc = fdtparse_get_interrupt_parent(fdt, nodeoffset);
    if (intc < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(fdt, intc, AM335X_INTC_COMPAT)) {
        /* We don't know how to initialize this! */
        return NULL;
    }

    regs = fdtparse_get_addr32(fdt, intc, "regs");
    if (!regs) {
        return NULL;
    }

    return regs;
}

int am335x_interrupt_register(const void *fdt, int nodeoffset, uint8_t num,
                              void (*func)(void*), void *data) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* Set interrupt handler */
    interrupt_handlers[num] = func;
    interrupt_handler_data[num] = data;

    return 0;
}

int am335x_interrupt_unregister(const void *fdt, int nodeoffset, uint8_t num) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* Set interrupt handler */
    interrupt_handlers[num] = unhandled_interrupt;
    interrupt_handler_data[num] = NULL;

    return 0;
}

int am335x_interrupt_priority(const void *fdt, int nodeoffset, uint8_t num,
                              uint8_t priority) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* Set priority */
    raw_mem_set_mask(&regs->ilr[num], AM335X_INTC_ILR_PRIORITY_MASK,
                     priority << AM335X_INTC_ILR_PRIORITY_SHIFT);

    return 0;
}

int am335x_interrupt_fiq(const void *fdt, int nodeoffset, uint8_t num,
                         uint8_t fiq) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* Set FIQ/IRQ */
    raw_mem_set_mask(&regs->ilr[num], AM335X_INTC_ILR_FIQ,
                     fiq ? 1 : 0);

    return 0;
}

int am335x_interrupt_enable(const void *fdt, int nodeoffset, uint8_t num) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    int bank, bit;

    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* There are 32 interrupts per bank */
    bank = num / 32;
    bit = num % 32;

    /* Enable the interrupt */
    raw_mem_write(&regs->bank[bank].mir_clear, 1 << bit);

    return 0;
}

int am335x_interrupt_disable(const void *fdt, int nodeoffset, uint8_t num) {
    struct am335x_intc *regs = interrupt_parent_regs(fdt, nodeoffset);
    int bank, bit;

    if (regs != primary_intc) {
        fprintf(stderr, "Only primary interrupt controller supported\r\n");
        return -1;
    }

    /* There are 32 interrupts per bank */
    bank = num / 32;
    bit = num % 32;

    /* Disable the interrupt */
    raw_mem_write(&regs->bank[bank].mir_set, 1 << bit);

    return 0;
}

/*
 * Initialize primary interrupt controller
 *
 * Initializes controller specified in the root interrupt-parent property.
 *
 * Initializes the primary intc registers used by the IRQ and FIQ handlers,
 */
static int am335x_intc_init(void) {
    const void *fdt = fdtparse_get_blob();
    int root;

    root = fdt_path_offset(fdt, "/");
    if (root < 0) {
        return -1;
    }

    primary_intc = interrupt_parent_regs(fdt, root);
    if (!primary_intc) {
        return -1;
    }

    /* Enable AUTOIDLE power saving */
    raw_mem_set_bits(&primary_intc->sysconfig, AM335X_INTC_SYSCONFIG_AUTOIDLE);

    return 0;
}
EARLY_INITIALIZER(am335x_intc_init)
