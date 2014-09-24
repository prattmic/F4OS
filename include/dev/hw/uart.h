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

#ifndef DEV_HW_UART_H_INCLUDED
#define DEV_HW_UART_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>
#include <kernel/mutex.h>

struct uart {
    void *priv;
    struct obj obj;
};

static inline struct uart *to_uart(struct obj *o) {
    return (struct uart *) container_of(o, struct uart, obj);
}

struct uart_ops {
    /**
     * Initialize UART peripheral
     *
     * Initialize UART peripheral to be ready for communication.  Sets
     * up UART hardware registers or software emulation, and prepares
     * any internal data structures.  Returns success if peripheral
     * is already initialized.
     *
     * Calling this function is not required.  The peripheral will be
     * lazily initialized on first use.
     *
     * @param uart  UART peripheral to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct uart *);
    /**
     * Deinitialize UART peripheral
     *
     * Frees any internal data structures and optionally powers down
     * hardware peripherals.
     *
     * Calling this function on a non-initialized peripheral has no effect.
     *
     * @param uart  UART peripheral to deinitialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct uart *);
    /**
     * Get current baud rate
     *
     * @param uart  UART to get baud rate of
     *
     * @returns Currently configured baud rate, negative on error
     */
    int     (*get_baud_rate)(struct uart *);
    /**
     * Set baud rate
     *
     * Attempt to set baud rate.  If exact baud rate cannot be achieved,
     * the closest possible will be used, and returned.
     *
     * @param uart  UART to set baud rate of
     * #param baud  Baud rate to attempt to set
     *
     * @returns Set baud rate, which may differ from passed, negative on error
     */
    int     (*set_baud_rate)(struct uart *, unsigned int);
    /**
     * Read from UART
     *
     * Read up to num bytes from the UART into buf.  read() does not block.
     * It will return immediately if blocking would be required.  As such,
     * multiple calls may be necessary to complete reading.
     *
     * @param uart  UART to read from
     * @param buf   Buffer to read into.  Must hold at least num bytes
     * @param num   Number of bytes to read
     *
     * @returns number of bytes read, or negative on error
     */
    int     (*read)(struct uart *, char *, size_t);
    /**
     * Write to UART
     *
     * Write up to num bytes to the UART from buf.  write() does not block.
     * It will return immediately if blocking would be required.  As such,
     * multiple calls may be necessary to complete reading.
     *
     * @param uart  UART to write to
     * @param buf   Buffer to write from.  Must have at least num bytes
     * @param num   Number of bytes to write
     *
     * @returns number of bytes written, or negative on error
     */
    int     (*write)(struct uart *, char *, size_t);
};

extern struct class uart_class;

#endif
