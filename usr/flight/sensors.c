#include <stdio.h>
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

/* stderr on the PX4 is a bit annoying to connect to.
 * Change this to stdout to put errors on it instead. */
#define ERROR_RD    stdout

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
    uint8_t mag_good = 0;
    uint8_t mpu_good = 0;

    if (mag_rd > 0) {
        if (read_px4_hmc5883(mag_rd, &holding.mag)) {
            fprintf(ERROR_RD, "WARNING: Failed to read magnetometer.  Closing device.\r\n");
            close(mag_rd);
            mag_rd = -1;
        }
        else {  /* Success */
            mag_good = 1;
        }
    }
    else {
        fprintf(ERROR_RD, "INFO: Magnetometer closed, attempting to open.\r\n");
        mag_rd = open_px4_hmc5883();
        if (mag_rd < 0) {
            fprintf(ERROR_RD, "ERROR: Failed to open magnetometer.\r\n");
        }
    }

    if (mpu_rd > 0) {
        if (read_px4_mpu6000(mpu_rd, &holding.accel, &holding.gyro, &holding.temp)) {
            fprintf(ERROR_RD, "WARNING: Failed to read MPU6000.  Closing device.\r\n");
            close(mpu_rd);
            mpu_rd = -1;
        }
        else {  /* Success */
            mpu_good = 1;
        }
    }
    else {
        fprintf(ERROR_RD, "INFO: MPU6000 closed, attempting to open.\r\n");
        mpu_rd = open_px4_mpu6000();
        if (mpu_rd < 0) {
            fprintf(ERROR_RD, "ERROR: Failed to open MPU6000.\r\n");
        }
    }

    acquire(&sensor_semaphore);

    if (mag_good) {
        memcpy(&current_sensor_readings.mag, &holding.mag, sizeof(holding.mag));
        current_sensor_readings.new_mag = 1;
    }

    if (mpu_good) {
        memcpy(&current_sensor_readings.accel, &holding.accel, sizeof(holding.accel));
        memcpy(&current_sensor_readings.gyro, &holding.gyro, sizeof(holding.gyro));
        current_sensor_readings.temp = holding.temp;
        current_sensor_readings.new_accel = 1;
        current_sensor_readings.new_gyro = 1;
        current_sensor_readings.new_temp = 1;
    }

    release(&sensor_semaphore);
}

/* Reading the baro requires 20ms of sleeping, so it gets its own task. */
void read_baro(void) {
    struct barometer holding;

    if (baro_rd > 0) {
        if (read_px4_ms5611(baro_rd, &holding)) {
            fprintf(ERROR_RD, "WARNING: Failed to read barometer.  Closing device.\r\n");
            close(baro_rd);
            baro_rd = -1;
            return;
        }
    }
    else {
        fprintf(ERROR_RD, "INFO: Barometer closed, attempting to open.\r\n");
        baro_rd = open_px4_ms5611();
        if (baro_rd < 0) {
            fprintf(ERROR_RD, "ERROR: Failed to open barometer.\r\n");
        }
        return;
    }

    acquire(&sensor_semaphore);

    memcpy(&current_sensor_readings.baro, &holding, sizeof(struct barometer));
    current_sensor_readings.new_baro = 1;

    release(&sensor_semaphore);
}
