#include <string.h>
#include <kernel/fault.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <dev/periph/px4_ms5611.h>
#include <dev/periph/px4_hmc5883.h>
#include <dev/periph/px4_mpu6000.h>

#include "sensors.h"

struct sensors current_sensor_readings = {};
struct semaphore sensor_semaphore = {
    .lock = 0,
    .waiting = NULL,
    .held_by = NULL,
};

rd_t baro_rd = -1;
rd_t mag_rd = -1;
rd_t mpu_rd = -1;

void read_sensors(void);
void read_baro(void);

/* Open all of the sensors resources, so that read_sensors()
 * can inherit them. */
void init_sensors(void) {
    baro_rd = open_px4_ms5611();
    if (baro_rd < 0) {
        goto fail;
    }

    mag_rd = open_px4_hmc5883();
    if (mag_rd < 0) {
        goto fail;
    }

    mpu_rd = open_px4_mpu6000();
    if (mpu_rd < 0) {
        goto fail;
    }

    /* Read sensors ever 10ms */
    new_task(&read_sensors, 1, 40);

    /* Read baro every 50ms, because it requires 20ms of sleeping */
    new_task(&read_baro, 1, 200);

    return;

fail:
    panic_print("Unable to open sensors.");
}

void read_sensors(void) {
    struct sensors holding;
    uint8_t mag_good = 1;
    uint8_t mpu_good = 1;

    if (read_px4_hmc5883(mag_rd, &holding.mag)) {
        mag_good = 0;
        printk("WARNING: Failed to read magnetometer.\r\n");
    }

    if (read_px4_mpu6000(mpu_rd, &holding.accel, &holding.gyro, &holding.temp)) {
        mpu_good = 0;
        printk("WARNING: Failed to read MPU6000.\r\n");
    }

    acquire(&sensor_semaphore);

    if (mag_good) {
        memcpy(&current_sensor_readings.mag, &holding.mag, sizeof(holding.mag));
    }

    if (mpu_good) {
        memcpy(&current_sensor_readings.accel, &holding.accel, sizeof(holding.accel));
        memcpy(&current_sensor_readings.gyro, &holding.gyro, sizeof(holding.gyro));
        current_sensor_readings.temp = holding.temp;
    }

    release(&sensor_semaphore);
}

/* Reading the baro requires 20ms of sleeping, so it gets its own task. */
void read_baro(void) {
    struct barometer holding;

    if (read_px4_ms5611(baro_rd, &holding)) {
        printk("WARNING: Failed to read barometer.\r\n");
        return;
    }

    acquire(&sensor_semaphore);

    memcpy(&current_sensor_readings.baro, &holding, sizeof(struct barometer));

    release(&sensor_semaphore);
}
