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

#ifndef DEV_CHAR_H_INCLUDED
#define DEV_CHAR_H_INCLUDED

/*
 * A character device is an abstract interface to devices that read and write
 * raw bytes.  Character devices are wrappers around a concrete base obj, such
 * as a UART or USB CDC device, and are thus not part of the standard "device"
 * subsystem.
 *
 * Upon creation, the base character device should already be initialized and
 * ready to use.  If unnecessary, the base obj may be NULL.
 */

#include <stdint.h>
#include <kernel/obj.h>

struct char_device {
    struct obj  obj;
    struct obj  *base;
    void        *priv;
};

#define to_char_device(__obj) container_of((__obj), struct char_device, obj)

struct char_ops {
    /**
     * Read from device
     *
     * Read up to num bytes from the device into buf.  read() blocks until
     * num bytes have been read, EOF occurs, or an error is encountered.
     *
     * @param char_device Character device to read from
     * @param buf   Buffer to read into.  Must hold at least num bytes
     * @param num   Number of bytes to read
     *
     * @returns number of bytes read, or negative on error
     */
    int     (*read)(struct char_device *, char *, size_t);
    /**
     * Write to device
     *
     * Write up to num bytes to the device from buf.  write() blocks until
     * num bytes have been written, or an error is encountered.
     *
     * @param char_device Character device to write to
     * @param buf   Buffer to write from.  Must have at least num bytes
     * @param num   Number of bytes to write
     *
     * @returns number of bytes written, or negative on error
     */
    int     (*write)(struct char_device *, char *, size_t);
    /**
     * Cleanup internal structures
     *
     * Frees internal data structures and prepares the obj to be destroyed.
     * Does not put the base obj.
     *
     * Should only be called from the char_device destructor.
     *
     * @param char_device Character device object to cleanup
     *
     * @returns zero on success, negative on error
     */
    int     (*_cleanup)(struct char_device *);
};

/**
 * Create a new char_device structure
 *
 * Destroyed by char_device destructor.
 *
 * @param base  Base concrete character device obj.  May be NULL.
 * @param ops   Character device ops
 */
struct char_device *char_device_create(struct obj *base,
                                       struct char_ops *ops);

#endif
