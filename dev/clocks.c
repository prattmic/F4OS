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
#include <stddef.h>
#include <dev/clocks.h>

LINKER_ARRAY_DECLARE(clocks)

static struct clock_driver *clocks_lookup(const void *fdt, int offset, const char *name) {
    const struct fdt_property *prop;
    int len, controller;
    fdt32_t *cell;
    uint32_t phandle;
    struct clock_driver *driver;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (len < 0) {
        return NULL;
    }

    /* Make sure there is at least one cell */
    if (len < sizeof(fdt32_t)) {
        return NULL;
    }

    /* The first cell is a phandle to the clock controller */
    cell = (fdt32_t *) prop->data;
    phandle = fdt32_to_cpu(cell[0]);

    controller = fdt_node_offset_by_phandle(fdt, phandle);
    if (controller < 0) {
        return NULL;
    }

    LINKER_ARRAY_FOR_EACH(clocks, driver) {
        if (!fdt_node_check_compatible(fdt, controller, driver->compat)) {
            return driver;
        }
    }

    /* Not found! */
    return NULL;
}

int clocks_enable(const void *fdt, int offset, const char *name) {
    struct clock_driver *driver = clocks_lookup(fdt, offset, name);
    if (!driver) {
        return -1;
    }

    return driver->enable(fdt, offset, name);
}

int clocks_disable(const void *fdt, int offset, const char *name) {
    struct clock_driver *driver = clocks_lookup(fdt, offset, name);
    if (!driver) {
        return -1;
    }

    return driver->disable(fdt, offset, name);
}

int clocks_set_param(const void *fdt, int offset, const char *name,
                     unsigned int param) {
    struct clock_driver *driver = clocks_lookup(fdt, offset, name);
    if (!driver) {
        return -1;
    }

    return driver->set_param(fdt, offset, name, param);
}
