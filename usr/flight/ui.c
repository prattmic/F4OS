#include <stdio.h>
#include <string.h>
#include <math.h>
#include <kernel/semaphore.h>
#include <dev/sensors.h>

#include "sensors.h"
#include "ui.h"

void ui(void) {
    printf("Press q to quit, any key to continue.\r\n");

    while (getc() != 'q') {
        struct sensors readings;

        acquire(&sensor_semaphore);
        memcpy(&readings, &current_sensor_readings, sizeof(struct sensors));
        release(&sensor_semaphore);

        float pitch = atan2(readings.accel.x, sqrtf(readings.accel.y*readings.accel.y + readings.accel.z*readings.accel.z));
        float roll = atan2(readings.accel.y, readings.accel.z);

        float uncomp_heading = atan2(readings.mag.y, readings.mag.x) * RAD_TO_DEG;
        uncomp_heading = uncomp_heading > 0 ? uncomp_heading : uncomp_heading + 360;

        printf("Accel: \t X: %fg \t Y: %fg \t Z: %fg \r\n", readings.accel.x, readings.accel.y, readings.accel.z);
        printf("Gyro: \t X: %fdps \t Y: %fdps \t Z: %fdps \r\n", readings.gyro.x, readings.gyro.y, readings.gyro.z);
        printf("Mag: \t X: %fG \t Y: %fG \t Z: %fG \r\n", readings.mag.x, readings.mag.y, readings.mag.z);
        printf("Baro: %fmb \t Baro temp: %fC \t MPU temp: %fC \r\n", readings.baro.pressure, readings.baro.temp, readings.temp);
        printf("Roll: %fdeg \t Pitch: %fdeg \r\n", roll*RAD_TO_DEG, pitch*RAD_TO_DEG);
        printf("Uncomp Heading: %fdeg \r\n", uncomp_heading);
    }
}
