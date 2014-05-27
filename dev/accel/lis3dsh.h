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

#ifndef DEV_ACCEL_LIS302DL_H
#define DEV_ACCEL_LIS302DL_H

/* When this bit is set, the register address on the accelerometer
 * will automatically increment for multi-byte reads/writes */
/* When set in register addres, perform a read instead of write */

/* g/digit in raw data */
#define LIS3DSH_SENSITIVITY     (0.00006f)

/* Registers */
#define LIS3DSH_WHOAMI     ((uint8_t) 0x0F)
#define LIS3DSH_CTRL4      ((uint8_t) 0x20)
#define LIS3DSH_CTRL6      ((uint8_t) 0x25)
#define LIS3DSH_OUTX_L     ((uint8_t) 0x28)
#define LIS3DSH_OUTY_L     ((uint8_t) 0x2a)
#define LIS3DSH_OUTZ_L     ((uint8_t) 0x2c)

#define SPI_READ            ((uint8_t) 0x80)
#define LIS3DSH_WHOAMI_VAL  ((uint8_t) 0x3f)        /* WHOAMI id value */

#define LIS3DSH_COMPAT "stmicro,lis3dsh"

struct lis3dsh {
    struct spi_dev spi_dev;
    int ready;
};

#endif
