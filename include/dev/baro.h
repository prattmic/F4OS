#ifndef DEV_BARO_H_INCLUDED
#define DEV_BARO_H_INCLUDED

#include <dev/device.h>
#include <kernel/class.h>
#include <kernel/obj.h>

struct baro_data {
    float pressure;     /* Pascal */
    float temperature;  /* deg C */
};

struct baro {
    struct device   device;
    struct obj      obj;
    void            *priv;
};

/* Takes obj and returns containing struct baro */
static inline struct baro *to_baro(struct obj *o) {
    return (struct baro *) container_of(o, struct baro, obj);
}

struct baro_ops {
    /**
     * Initialize barometer
     *
     * Power on barometer and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The barometer will be
     * initialized lazily on first use, if not already initialized.
     *
     * @param baro Barometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct baro *);
    /**
     * Deinitialize barometer
     *
     * Frees internal data structures and optionally powers down barometer.
     *
     * Calling this function on a non-initialized barometer has no effect.
     *
     * @param baro Barometer object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct baro *);
    /**
     * Determine if barometer reports temperature
     *
     * Many barometers report temperature along with pressure, as it is
     * usually necessary to determine the pressure.  If this function returns
     * true, the temperature field in the baro_data struct will be set by
     * get_data().
     *
     * @param baro  Barometer object to check
     *
     * @returns 1 if device reports temperature, 0 if not
     */
    int     (*has_temp)(struct baro *);
    /**
     * Get new barometer data
     *
     * Get a new reading from the barometer.  The result will be a new
     * value from the barometer, not a cached value.  However, the barometer
     * may do internal caching.
     *
     * The pressure returned from this function is in units of pascals.  If
     * the baro supports temperature (has_temp()), the temperature returned
     * will be in degrees Celsius.
     *
     * This function blocks until new data is available.
     *
     * @param baro  Barometer object to receive data from
     * @param data  baro_data struct to place data in
     *
     * @returns zero on success, negative on error
     */
    int     (*get_data)(struct baro *, struct baro_data *);
};

extern struct class baro_class;

#endif
