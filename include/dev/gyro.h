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

#ifndef DEV_GYRO_H_INCLUDED
#define DEV_GYRO_H_INCLUDED

#include <dev/device.h>
#include <kernel/obj.h>
#include <kernel/class.h>

struct gyro_raw_data {
    int x;
    int y;
    int z;
};

/* Should be in deg/s */
struct gyro_data {
    float x;
    float y;
    float z;
};

struct gyro {
    struct device   device;
    struct obj      obj;
    void            *priv;
};

/* Takes obj and returns containing struct gyro */
static inline struct gyro *to_gyro(struct obj *o) {
    return (struct gyro *) container_of(o, struct gyro, obj);
}

struct gyro_ops {
    /**
     * Initialize gyroscope
     *
     * Power on gyroscope and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The gyroscope will be
     * initialized lazily on first use, if not already initialized.
     *
     * @param gyro Gyroscope object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct gyro *);
    /**
     * Deinitialize gyroscope
     *
     * Frees internal data structures and optionally powers down gyroscope.
     *
     * Calling this function on a non-initialized gyroscope has no effect.
     *
     * @param gyro Gyroscope object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct gyro *);
    /**
     * Get new gyroscope data in deg/s
     *
     * Get a new reading from the gyroscope.  The result will be a new
     * value from the gyroscope, not a cached value.  However, the gyroscope
     * may do internal caching.
     *
     * The values returned from this function are in degrees per second.
     *
     * This function blocks until new data is available.
     *
     * @param gyro  Gyroscope object to receive data from
     * @param data  gyro_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_data)(struct gyro *, struct gyro_data *);
    /**
     * Get new gyroscope data
     *
     * Get a new reading from the gyroscope.  The result will be a new
     * value from the gyroscope, not a cached value.  However, the gyroscope
     * may do internal caching.
     *
     * The values returned from this function are raw reading from the
     * gyroscope,and the units will depend upon the specific gyroscope.
     *
     * This function blocks until new data is available.
     *
     * @param gyro  Gyroscope object to receive data from
     * @param data  gyro_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_raw_data)(struct gyro *, struct gyro_raw_data *);
};

extern struct class gyro_class;

#endif
