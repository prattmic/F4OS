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
#include <dev/clocks.h>
#include <dev/fdtparse.h>
#include <dev/raw_mem.h>
#include "clocks.h"

static int am335x_prcm_regs_and_offset(const void *fdt, int nodeoffset,
                                       const char *name, uintptr_t *regs,
                                       uint32_t *regoffset) {
    const struct fdt_property *clkprop;
    const struct fdt_property *regsprop;
    int len, controller;
    fdt32_t *cell;
    uint32_t phandle, banksize;

    clkprop = fdt_get_property(fdt, nodeoffset, name, &len);
    if (len < 0) {
        return -1;
    }

    /* Make sure there are two cells */
    if (len < 2*sizeof(fdt32_t)) {
        return -1;
    }

    cell = (fdt32_t *) clkprop->data;

    /* The first cell is a phandle to the clock controller */
    phandle = fdt32_to_cpu(cell[0]);

    /* The second cell is the clock controller register offset */
    *regoffset = fdt32_to_cpu(cell[1]);

    controller = fdt_node_offset_by_phandle(fdt, phandle);
    if (controller < 0) {
        return -1;
    }

    regsprop = fdt_get_property(fdt, controller, "regs", &len);
    if (len < 0) {
        return -1;
    }

    /* Make sure there are two cells */
    if (len < 2*sizeof(fdt32_t)) {
        return -1;
    }

    cell = (fdt32_t *) regsprop->data;

    /* The first cell is the base address */
    *regs = fdt32_to_cpu(cell[0]);

    /* The second cell is the register bank size */
    banksize = fdt32_to_cpu(cell[1]);

    /* Make sure offset is within register bank */
    if (*regoffset >= banksize) {
        return -1;
    }

    return 0;
}

int am335x_prcm_clock_enable(const void *fdt, int nodeoffset, const char *name) {
    uintptr_t reg_base;
    uint32_t offset;
    uint32_t *reg;
    int err;

    err = am335x_prcm_regs_and_offset(fdt, nodeoffset, name, &reg_base, &offset);
    if (err) {
        return err;
    }

    reg = (uint32_t *) (reg_base + offset);

    /* Enable module */
    raw_mem_set_mask(reg, CM_MODULEMODE, CM_MODULEMODE_ENABLED);

    /* Wait for module to enable */
    /* TODO: timeout */
    while ((raw_mem_read(reg) & CM_IDLEST) != CM_IDLEST_FUNC);

    return 0;
}

int am335x_prcm_clock_disable(const void *fdt, int nodeoffset, const char *name) {
    uintptr_t reg_base;
    uint32_t offset;
    uint32_t *reg;
    int err;

    err = am335x_prcm_regs_and_offset(fdt, nodeoffset, name, &reg_base, &offset);
    if (err) {
        return err;
    }

    reg = (uint32_t *) (reg_base + offset);

    /* Disable module */
    raw_mem_set_mask(reg, CM_MODULEMODE, CM_MODULEMODE_DISABLED);

    /* Wait for module to enable */
    /* TODO: timeout */
    while ((raw_mem_read(reg) & CM_IDLEST) != CM_IDLEST_DIS);

    return 0;
}

/*
 * Used for selecting clock sources in CM_DPLL.
 * The param specifies the register contents.
 */
int am335x_prcm_clock_set_param(const void *fdt, int nodeoffset,
                                const char *name, unsigned int param) {
    uintptr_t reg_base;
    uint32_t offset;
    uint32_t *reg;
    int err;

    err = am335x_prcm_regs_and_offset(fdt, nodeoffset, name, &reg_base, &offset);
    if (err) {
        return err;
    }

    reg = (uint32_t *) (reg_base + offset);

    /* Assign param to register */
    raw_mem_write(reg, param);

    return 0;
}

DECLARE_CLOCK_DRIVER(cmper) = {
    .compat = "ti,am33xx-prcm-cmper",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmwkup) = {
    .compat = "ti,am33xx-prcm-cmwkup",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmdpll) = {
    .compat = "ti,am33xx-prcm-cmdpll",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmmpu) = {
    .compat = "ti,am33xx-prcm-cmmpu",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmdevice) = {
    .compat = "ti,am33xx-prcm-cmdevice",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmrtc) = {
    .compat = "ti,am33xx-prcm-cmrtc",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmgfx) = {
    .compat = "ti,am33xx-prcm-cmgfx",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};

DECLARE_CLOCK_DRIVER(cmcefuse) = {
    .compat = "ti,am33xx-prcm-cmcefuse",
    .enable = am335x_prcm_clock_enable,
    .disable = am335x_prcm_clock_disable,
    .set_param = am335x_prcm_clock_set_param,
};
