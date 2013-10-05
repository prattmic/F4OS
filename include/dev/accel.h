#ifndef DEV_ACCEL_H_INCLUDED
#define DEV_ACCEL_H_INCLUDED
/*
 * New-style general accelerometer implementation.
 *
 * This provides a standard way to interact with an "accelerometer"
 * Specific acceleromters should:
 * ~ Have a way to initialize themselves
 * ~ Have a way to return an accel_data struct
 *
 */

#include <dev/device.h>
#include <kernel/obj.h>
#include <kernel/class.h>

struct accel_raw_data {
    int x;
    int y;
    int z;
};

/* Should be in g's */
struct accel_data {
    float x;
    float y;
    float z;
};

struct accel {
    struct device   device;
    struct obj      obj;
    void            *priv;
};

#define to_accel(__obj) container_of((__obj), struct accel, obj)

struct accel_ops {
    /**
     * Initialize accelerometer
     *
     * Power on accelerometer and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The acceleromter will be
     * initialized lazily on first use, if not already initialized.
     *
     * @param accel Accelerometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct accel *);
    /**
     * Deinitialize accelerometer
     *
     * Frees internal data structures and optionally powers down accelerometer.
     *
     * Calling this function on a non-initialized accelerometer has no effect.
     *
     * @param accel Accelerometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct accel *);
    /**
     * Get new accelerometer data in g's
     *
     * Get a new reading from the accelerometer.  The result will be a new
     * value from the accelerometer, not a cached value.  However, the accelerometer
     * may do internal caching.
     *
     * The values returned from this function are referenced to one g, approximately
     * 9.8m/s^2.
     *
     * This function blocks until new data is available.
     *
     * @param accel Accelerometer object to receive data from
     * @param data  accel_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_data)(struct accel *, struct accel_data *);
    /**
     * Get new accelerometer data
     *
     * Get a new reading from the accelerometer.  The result will be a new
     * value from the accelerometer, not a cached value.  However, the accelerometer
     * may do internal caching.
     *
     * The values returned from this function are raw reading from the accelerometer,
     * and the units will depend upon the specific accelerometer.
     *
     * This function blocks until new data is available.
     *
     * @param accel Accelerometer object to receive data from
     * @param data  accel_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_raw_data)(struct accel *, struct accel_raw_data *);
};

extern struct class accel_class;

#endif
