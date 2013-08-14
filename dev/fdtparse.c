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

#include <stdio.h>
#include <stdlib.h>
#include <libfdt.h>
#include <dev/fdtparse.h>

/* Get device tree blob from .dtb section */
extern void *_dtb_start;

static const void *fdt_blob = &_dtb_start;

const void *fdtparse_get_blob(void) {
    return fdt_blob;
}

int fdtparse_alias_offset(const void *fdt, const char *name) {
    const char *path;

    path = fdt_get_alias(fdt, name);
    if (!path) {
        return -FDT_ERR_NOTFOUND;
    }

    return fdt_path_offset(fdt, path);
}

int fdtparse_get_int(const void *fdt, int offset, const char *name, int *val) {
    const struct fdt_property *prop;
    int len;
    fdt32_t *cell;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (len < 0) {
        return len;
    }
    else if (len < sizeof(int)) {
        return -FDT_ERR_NOTFOUND;
    }

    cell = (fdt32_t *) prop->data;

    *val = fdt32_to_cpu(cell[0]);

    return 0;
}

void *fdtparse_get_addr32(const void *fdt, int offset, const char *name) {
    const struct fdt_property *prop;
    int len;
    fdt32_t *cell;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (len < sizeof(fdt32_t)) {
        return NULL;
    }

    cell = (fdt32_t *) prop->data;

    return (void *)fdt32_to_cpu(cell[0]);
}

int fdtparse_get_gpio(const void *fdt, int offset, const char *name,
                      struct fdt_gpio *gpio) {
    const struct fdt_property *prop;
    int len;
    fdt32_t *cell;

    prop = fdt_get_property(fdt, offset, name, &len);
    if (len < 0) {
        return len;
    }

    /* GPIO cells have 3 fields */
    if (len != 3*sizeof(uint32_t)) {
        return -FDT_ERR_BADLAYOUT;
    }

    cell = (fdt32_t *) prop->data;

    /* cell[0] is gpio path, cell[1] is number, cell[2] is flags */
    gpio->gpio = fdt32_to_cpu(cell[1]);
    gpio->flags = fdt32_to_cpu(cell[2]);

    return 0;
}

char *fdtparse_get_path(const void *fdt, int offset) {
    int err, size;
    char *path = NULL;

    /* Make an arbitrary best guess at the max path size */
    size = 32;

    do {
        /*
         * There wasn't enough space last time,
         * free that attempt and try again.
         */
        if (path) {
            free(path);
            size += 32;
        }

        path = malloc(size);
        if (!path) {
            fprintf(stderr, "%s: Unable to allocate %d bytes for path\n",
                    __func__, size);
            return NULL;
        }

        err = fdt_get_path(fdt, offset, path, size);
    } while (err == -FDT_ERR_NOSPACE);

    if (err) {
        free(path);
        return NULL;
    }

    return path;
}

int fdtparse_get_interrupt_parent(const void *fdt, int nodeoffset) {
    int len;
    const struct fdt_property *interrupt_parent;
    fdt32_t *cell;
    uint32_t parent_phandle;
    int parent_offset;

    /* Use "interrupt-parent" from node, or node's parent */
    do {
        interrupt_parent = fdt_get_property(fdt, nodeoffset,
                                            "interrupt-parent", &len);
    } while ((len < 0) &&
             (nodeoffset = fdt_parent_offset(fdt, nodeoffset)) >= 0);

    if (len < 0) {
        return len;
    }

    /* Cell must be big enough to hold phandle */
    if (len < sizeof(fdt32_t)) {
        return -FDT_ERR_BADPHANDLE;
    }

    cell = (fdt32_t *) interrupt_parent->data;
    parent_phandle = fdt32_to_cpu(cell[0]);
    parent_offset = fdt_node_offset_by_phandle(fdt, parent_phandle);
    if (parent_offset < 0) {
        return parent_offset;
    }

    return parent_offset;
}
