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
#define ADDR_INC (1 << 6)

/* When set in register addres, perform a read instead of write */
#define SPI_READ    (1 << 7)

/* g/digit in raw data */
#define LIS302DL_SENSITIVITY     (0.018f)

/* Registers */
#define LIS302DL_WHOAMI     ((uint8_t) 0x0F)
#define LIS302DL_CTRL1      ((uint8_t) 0x20)
#define LIS302DL_CTRL2      ((uint8_t) 0x21)
#define LIS302DL_CTRL3      ((uint8_t) 0x22)
#define LIS302DL_HP_RST     ((uint8_t) 0x23)
#define LIS302DL_STATUS     ((uint8_t) 0x27)
#define LIS302DL_OUTX       ((uint8_t) 0x29)
#define LIS302DL_OUTY       ((uint8_t) 0x2B)
#define LIS302DL_OUTZ       ((uint8_t) 0x2D)

#define LIS302DL_WHOAMI_VAL ((uint8_t) 0x3B)        /* WHOAMI id value */

#define LIS302DL_CTRL1_XEN  ((uint8_t) (1 << 0))    /* X axis enable */
#define LIS302DL_CTRL1_YEN  ((uint8_t) (1 << 1))    /* Y axis enable */
#define LIS302DL_CTRL1_ZEN  ((uint8_t) (1 << 2))    /* Z axis enable */
#define LIS302DL_CTRL1_STM  ((uint8_t) (1 << 3))    /* Self-test M */
#define LIS302DL_CTRL1_STP  ((uint8_t) (1 << 4))    /* Self-test P */
#define LIS302DL_CTRL1_FS   ((uint8_t) (1 << 5))    /* Full-scale selection */
#define LIS302DL_CTRL1_PD   ((uint8_t) (1 << 6))    /* Power-down control (0=off) */
#define LIS302DL_CTRL1_DR   ((uint8_t) (1 << 7))    /* Data rate (0=100Hz, 1=400Hz) */

#endif
