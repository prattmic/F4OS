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

#include <dev/char.h>
#include <dev/hw/uart.h>
#include <kernel/class.h>
#include <kernel/obj.h>
#include <mm/mm.h>

static void uart_dtor(struct obj *o);

struct obj_type uart_type_s  = {
    .offset = offset_of(struct uart, obj),
    .dtor = &uart_dtor,
};

struct class uart_class = INIT_CLASS(uart_class, "uart", &uart_type_s);

static void uart_dtor(struct obj *o) {
    struct uart *uart;
    struct uart_ops *ops;

    assert_type(o, &uart_type_s);
    uart = to_uart(o);
    ops = (struct uart_ops *) o->ops;
    ops->deinit(uart);

    /* Deinitialize, but don't destroy the UART */
}

static int uart_read(struct char_device *dev, char *buf, size_t num) {
    struct uart *uart;
    struct uart_ops *ops;

    if (!dev) {
        return -1;
    }

    uart = to_uart(dev->base);
    ops = dev->base->ops;

    return ops->read(uart, buf, num);
}

static int uart_write(struct char_device *dev, const char *buf, size_t num) {
    struct uart *uart;
    struct uart_ops *ops;

    if (!dev) {
        return -1;
    }

    uart = to_uart(dev->base);
    ops = dev->base->ops;

    return ops->write(uart, buf, num);
}

static int uart_cleanup(struct char_device *dev) {
    /* Nothing special to do that won't be done by the uart destructor */
    return 0;
}

static struct char_ops uart_char_ops = {
    .read = uart_read,
    .write = uart_write,
    ._cleanup = uart_cleanup,
};

struct char_device *uart_to_char_device(struct obj *obj) {
    assert_type(obj, &uart_type_s);
    return char_device_create(obj, &uart_char_ops);
}

DECLARE_CHAR_CONVERSION(uart) = {
    .type = &uart_type_s,
    .cast = uart_to_char_device,
};
