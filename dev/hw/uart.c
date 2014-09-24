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

#include <dev/hw/uart.h>
#include <kernel/class.h>
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
