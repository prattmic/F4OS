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
#include <dev/device.h>
#include <dev/mag.h>
#include <dev/shared_deq.h>
#include <kernel/obj.h>

#include "sensors.h"

DEFINE_SHARED_DEQ(mag_queue);

static struct mag *setup_mag(void) {
    struct obj *obj;

    obj = device_get("/i2c@40005800/hmc5883l@1E");
    if (!obj) {
        printf("Failed to get mag\r\n");
        abort();
    }

    return to_mag(obj);
}

void read_mag(void) {
    static struct mag *mag = NULL;
    struct mag_ops *ops;
    struct mag_queue_entry *entry;
    int err;

    if (!mag) {
        mag = setup_mag();
    }

    ops = mag->obj.ops;

    entry = malloc(sizeof(*entry));
    if (!entry) {
        printf("Failed to allocate mag queue entry\r\n");
        return;
    }

    err = ops->get_data(mag, &entry->data);
    if (err) {
        printf("Failed to get mag data: %d\r\n", err);
        return;
    }

    sdeq_add(&mag_queue, entry);
}
