/*
 * Copyright (C) 2013 F4OS Authors
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

#ifndef DEV_HW_I2C_H_INCLUDED
#define DEV_HW_I2C_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>
#include <kernel/semaphore.h>

struct i2c {
    uint8_t             num;
    struct semaphore    lock;
    void                *priv;
    struct obj          obj;
};

/* Takes obj and returns containing struct i2c */
static inline struct i2c *to_i2c(struct obj *o) {
    return (struct i2c *) container_of(o, struct i2c, obj);
}

struct i2c_ops {
    /**
     * Initialize I2C peripheral
     *
     * Initialize I2C peripheral to be ready for communication.  Sets
     * up I2C hardware registers or software emulation, and prepares
     * any internal data structures.  Returns success if peripheral
     * is already initialized.
     *
     * Calling this function is not required.  The peripheral will be
     * lazily initialized on first use.
     *
     * @param i2c   I2C peripheral to initialize
     *
     * @returns zero on success, negative on error
     */
    int         (*init)(struct i2c *);
    /**
     * Deinitialize I2C peripheral
     *
     * Frees any internal data structures and optionally powers down
     * hardware peripherals.
     *
     * Calling this function on a non-initialized peripheral has no effect.
     *
     * @param i2c   I2C peripheral to deinitialize
     *
     * @returns zero on success, negative on error
     */
    int         (*deinit)(struct i2c *);
    /**
     * Write data to I2C device.
     *
     * Write num bytes of data to I2C device dev on port i2c, setting
     * the device chip select automatically.
     *
     * The 7-bit device address should be given, the lower RW bit will be set
     * outomatically.
     *
     * @param i2c   I2C peripheral the device is connected to
     * @param addr  Address of I2C device
     * @param data  Data to write
     * @param num   Number of bytes of data to write
     *
     * @returns bytes written successfully, negative on error
     */
    int         (*write)(struct i2c *, uint8_t, uint8_t *, uint32_t);
    /**
     * Read data from I2C device.
     *
     * Read num bytes of data from I2C device dev on port i2c.
     *
     * The 7-bit device address should be given, the lower RW bit will be set
     * outomatically.
     *
     * @param i2c   I2C peripheral the device is connected to
     * @param addr  Address of I2C device
     * @param data  Buffer to read data into
     * @param num   Number of bytes of data to read
     *
     * @returns bytes read into buffer, negative on error
     */
    int         (*read)(struct i2c *, uint8_t, uint8_t *, uint32_t);
};

extern struct class i2c_class;

#endif
