/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

#include <stddef.h>
#include <stdio.h>
#include <dev/char.h>
#include <dev/device.h>
#include <dev/hw/usbdev.h>
#include <kernel/init.h>
#include <kernel/mutex.h>
#include <mm/mm.h>
#include "usbdev_desc.h"
#include "usbdev_internals.h"

struct usb {
    struct mutex read_mutex;
    struct mutex write_mutex;
};

static int usb_write(struct char_device *dev, const char *buf, size_t num) {
    struct usb *usb;
    int ret;

    if (!usb_ready || !dev) {
        return -1;
    }

    usb = dev->priv;

    acquire(&usb->write_mutex);

    ret = usbdev_write(&ep_tx, (const uint8_t*)buf, num);

    release(&usb->write_mutex);

    return ret;
}

static int usb_read(struct char_device *dev, char *buf, size_t num) {
    struct usb *usb;
    int total = 0;

    if (!usb_ready || !dev) {
        return -1;
    }

    usb = dev->priv;

    acquire(&usb->read_mutex);

    for (int i = 0; i < num; i++) {
        if (ring_buf_empty(&ep_rx.rx)) {
            break;
        }

        buf[i] = (char) ep_rx.rx.buf[ep_rx.rx.start];
        ep_rx.rx.start = (ep_rx.rx.start + 1) % ep_rx.rx.len;
        total++;
    }

    release(&usb->read_mutex);

    return total;
}

static int usb_cleanup(struct char_device *dev) {
    if (!dev) {
        return -1;
    }

    kfree(dev->priv);

    return 0;
}

static struct char_ops usb_ops = {
    .read = usb_read,
    .write = usb_write,
    ._cleanup = usb_cleanup,
};

static struct mutex driver_mutex = INIT_MUTEX;

static int stm32f4_usb_probe(const char *name) {
    /* Statically built driver always exists */
    return 1;
}

static struct obj *stm32f4_usb_ctor(const char *name) {
    struct usb *usb;
    struct char_device *dev;

    usb = kmalloc(sizeof(*usb));
    if (!usb) {
        return NULL;
    }

    init_mutex(&usb->read_mutex);
    init_mutex(&usb->write_mutex);

    dev = char_device_create(NULL, &usb_ops);
    if (!dev) {
        return NULL;
    }

    dev->priv = usb;

    return &dev->obj;
}

static int stm32f4_usb_register(void) {
    struct device_driver *new = kmalloc(sizeof(*new));
    if (!new) {
        fprintf(stderr, "%s: Unable to allocate device driver", __func__);
        return -1;
    }

    new->name = "stm32f4-static-usb";
    new->probe = stm32f4_usb_probe;
    new->ctor = stm32f4_usb_ctor;
    new->class = NULL;
    new->mut = &driver_mutex;

    device_driver_register(new);

    return 0;
}
CORE_INITIALIZER(stm32f4_usb_register)
