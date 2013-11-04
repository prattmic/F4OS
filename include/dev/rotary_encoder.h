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

#ifndef DEV_ROTARY_ENCODER_H_INCLUDED
#define DEV_ROTARY_ENCODER_H_INCLUDED

#include <stdint.h>
#include <dev/device.h>
#include <kernel/class.h>
#include <kernel/obj.h>

struct rotary_encoder_raw_data {
    uint32_t angle;
};

/* Should be in radians */
struct rotary_encoder_data {
    float angle;
};

struct rotary_encoder {
    struct device   device;
    struct obj      obj;
    void            *priv;
};

/* Takes obj and returns containing struct rotary_encoder */
static inline struct rotary_encoder *to_rotary_encoder(struct obj *o) {
    return (struct rotary_encoder *) container_of(o, struct rotary_encoder, obj);
}

struct rotary_encoder_ops {
    /**
     * Initialize rotary encoder
     *
     * Power on rotary encoder and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The rotary encoder will be
     * initialized lazily on first use, if not already initialized.
     *
     * @param rotary_encoder Rotary encoder object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct rotary_encoder *);
    /**
     * Deinitialize rotary encoder
     *
     * Frees internal data structures and optionally powers down rotary encoder.
     *
     * Calling this function on a non-initialized rotary encoder has no effect.
     *
     * @param rotary_encoder Rotary encoder object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct rotary_encoder *);
    /**
     * Get new rotary encoder data in radians
     *
     * Get a new reading from the rotary encoder.  The result will be a new
     * angle value from the rotary encoder, not a cached value.  However,
     * the rotary encoder may do internal caching.
     *
     * The position returned from this function is in units of radians.
     *
     * This function blocks until new data is available.
     *
     * @param rotary_encoder Rotary encoder object to receive data from
     * @param data  rotary_encoder_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_data)(struct rotary_encoder *, struct rotary_encoder_data *);
    /**
     * Get new rotary encoder data
     *
     * Get a new reading from the rotary encoder.  The result will be a new
     * angle value from the rotary encoder, not a cached value.  However,
     * the rotary encoder may do internal caching.
     *
     * The values returned from this function are raw reading from the rotary
     * encoder, and the units will depend upon the specific rotary encoder.
     *
     * This function blocks until new data is available.
     *
     * @param rotary_encoder Rotary encoder object to receive data from
     * @param data  rotary_encoder_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_raw_data)(struct rotary_encoder *,
                            struct rotary_encoder_raw_data *);
    /**
     * Calibrate rotary encoder
     *
     * Perform a calibration routine for the rotary encoder.  The behavior
     * of this function is implementation dependent and should be documented
     * with the rotary encoder driver.  It may perform a runtime calibration,
     * a one time calibration, software calibration, or do nothing at all.
     *
     * @param rotary_encoder Rotary encoder object
     * @param value Implementation defined argument to calibration routine
     *
     * @returns zero on success, negative on error
     */
    int     (*calibrate)(struct rotary_encoder *, uint32_t value);
    /**
     * Encoder status
     *
     * Get status information from the rotary encoder.  The return value of
     * this function is implementation defined, as it is intended to return
     * values specific to the device.
     *
     * @param rotary_encoder Rotary encoder object
     *
     * @returns implementation defined
     */
    int     (*status)(struct rotary_encoder *);
};

extern struct class rotary_encoder_class;

#endif
