#include <stdio.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <arch/chip/dev/periph/px4_mpu6000.h>

#include "app.h"

void px4_accel_gyro(int argc, char **argv) {
    rd_t rd = open_px4_mpu6000();
    if (rd < 0) {
        printf("Error: unable to open mpu6000.\r\n");
        return;
    }

    printf("Press q to quit, any key to continue.\r\n");

    struct accelerometer accel_data;
    struct gyro gyro_data;
    float temp;

    while (getc() != 'q') {
        if (!read_px4_mpu6000(rd, &accel_data, &gyro_data, &temp)) {
            printf("Accel: X: %fg \t Y: %fg \t Z: %fg \t Gyro: X: %fdps \t Y: %fdps \t Z: %fdps at %fC \r\n", accel_data.x, accel_data.y, accel_data.z, gyro_data.x, gyro_data.y, gyro_data.z, temp);
        }
        else {
            printf("Unable to read mpu6000.\r\n");
        }
    }

    close(rd);
}
DEFINE_APP(px4_accel_gyro)
