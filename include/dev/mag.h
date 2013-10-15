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

#ifndef DEV_MAG_H_INCLUDED
#define DEV_MAG_H_INCLUDED

#include <dev/device.h>
#include <kernel/class.h>
#include <kernel/obj.h>

struct mag_raw_data {
    int x;
    int y;
    int z;
};

/* Should be in gauss */
struct mag_data {
    float x;
    float y;
    float z;
};

struct mag {
    struct device   device;
    struct obj      obj;
    void            *priv;
};

/* Takes obj and returns containing struct mag */
static inline struct mag *to_mag(struct obj *o) {
    return (struct mag *) container_of(o, struct mag, obj);
}

struct mag_ops {
    /**
     * Initialize magnetometer
     *
     * Power on magnetometer and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The mageromter will be
     * initialized lazily on first use, if not already initialized.
     *
     * @param mag Magnetometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct mag *);
    /**
     * Deinitialize magnetometer
     *
     * Frees internal data structures and optionally powers down magnetometer.
     *
     * Calling this function on a non-initialized magnetometer has no effect.
     *
     * @param mag Magnetometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct mag *);
    /**
     * Get new magnetometer data in gauss
     *
     * Get a new reading from the magnetometer.  The result will be a new
     * value from the magnetometer, not a cached value.  However, the magnetometer
     * may do internal caching.
     *
     * The values returned from this function are in units of gauss.
     *
     * This function blocks until new data is available.
     *
     * @param mag Magnetometer object to receive data from
     * @param data  mag_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_data)(struct mag *, struct mag_data *);
    /**
     * Get new magnetometer data
     *
     * Get a new reading from the magnetometer.  The result will be a new
     * value from the magnetometer, not a cached value.  However, the magnetometer
     * may do internal caching.
     *
     * The values returned from this function are raw reading from the magnetometer,
     * and the units will depend upon the specific magnetometer.
     *
     * This function blocks until new data is available.
     *
     * @param mag Magnetometer object to receive data from
     * @param data  mag_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_raw_data)(struct mag *, struct mag_raw_data *);
};

extern struct class mag_class;

#endif
