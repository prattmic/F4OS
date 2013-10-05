#ifndef DEV_MPU6000_CLASS_H_INCLUDED
#define DEV_MPU6000_CLASS_H_INCLUDED

#include <stdint.h>
#include <dev/device.h>
#include <kernel/class.h>
#include <kernel/obj.h>
#include <kernel/semaphore.h>

struct mpu6000 {
    struct device       device;
    struct obj          obj;
    uint8_t             ready;
    uint8_t             gyro_in_use;
    uint8_t             accel_in_use;
    struct semaphore    lock;
    void                *priv;
};

/* Takes obj and returns containing struct mpu6000 */
static inline struct mpu6000 *to_mpu6000(struct obj *o) {
    return (struct mpu6000 *) container_of(o, struct mpu6000, obj);
}

struct mpu6000_ops {
    /**
     * Initialize MPU6000
     *
     * Power on MPU6000 and prepare for data collection.  Initialize
     * any internal data structures and prepare object for other methods
     * to be called.  Returns success if already initialized.
     *
     * Calling this function is not required.  The MPU6000 will be
     * initialized lazily on first use, if not already initialized.
     *
     * Note: To read sensors, they must be enabled with the methods below.
     *
     * @param mpu   MPU6000 object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*init)(struct mpu6000 *);
    /**
     * Deinitialize MPU6000
     *
     * Frees internal data structures and optionally powers down MPU6000.
     *
     * Calling this function on a non-initialized MPU6000 has no effect.
     *
     * @param mpu   MPU6000 object to initialize
     *
     * @returns zero on success, negative on error
     */
    int     (*deinit)(struct mpu6000 *);
    /* Write register
     *
     * Write a value to an internal MPU6000 register.
     *
     * @param mpu   MPU6000 object to write to
     * @param reg   Register address to write to
     * @param val   Value to write to address
     *
     * @returns zero on success, negative on error
     */
    int     (*write_reg)(struct mpu6000 *, uint8_t, uint8_t);
    /* Read registers
     *
     * Read values from internal MPU6000 registers.
     *
     * Reads multiple consecutive registers into a buffer.  The buffer
     * must be large enough to hold the number of bytes specified.
     *
     * @param mpu   MPU6000 object to write to
     * @param reg   Initial register address to read from
     * @param buf   Buffer to read register contents into
     * @param num   Number of consecutive registers to read
     *
     * @returns number of bytes successfully written
     */
    int     (*read_regs)(struct mpu6000 *, uint8_t, uint8_t *, uint8_t);
    /**
     * Enable accelerometer
     *
     * Enable/disable use of the MPU6000 accelerometer.  The accelerometer
     * must be enabled before attempting to read it.
     *
     * If both the acclerometer and gyroscope are disabled, the device may
     * enter a low power state.  This function should be used instead of
     * writing the registers directly, as it provides an indication when
     * the device may enter a low power state.
     *
     * @param mpu       MPU6000 object to modify
     * @param enable    Enable accelerometer if 1, disable if 0
     *
     * @returns zero on success, negative on error
     */
    int     (*enable_accelerometer)(struct mpu6000 *, int);
    /**
     * Enable gyroscope
     *
     * Enable/disable use of the MPU6000 gyroscope.  The gyroscope
     * must be enabled before attempting to read it.
     *
     * If both the acclerometer and gyroscope are disabled, the device may
     * enter a low power state.  This function should be used instead of
     * writing the registers directly, as it provides an indication when
     * the device may enter a low power state.
     *
     * @param mpu       MPU6000 object to modify
     * @param enable    Enable gyroscope if 1, disable if 0
     *
     * @returns zero on success, negative on error
     */
    int     (*enable_gyroscope)(struct mpu6000 *, int);
};

extern struct class mpu6000_class;

#endif
