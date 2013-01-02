#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <kernel/semaphore.h>
#include <dev/sensors.h>

#include "sensors.h"
#include "ui.h"

/* I'll need this citation later */
/* Beard, McLain. Navigation, Guidance, and Control of Small Unmanned Aircraft.
 * Equations 8.12 and 8.13 */

void gyro_bias(struct gyro *bias);
void print_readings(struct gyro *bias);

void ui(void) {
    /* Wait for stdin/stdout */
    while (getc() < 0);

    printf("Press q to quit, c to calibrate gyro, any other key to continue.\r\n");

    struct gyro bias = {
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    };

    while (1) {
        int c = getc();
        if (c < 0) {
            printf("Input error.\r\n");
            continue;
        }

        switch (c) {
        case 'q':
            printf("You're on your own now!\r\n");
            return;
        case 'c':
            printf("Finding gyro bias.  Do not move device!\r\n");
            gyro_bias(&bias);
            printf("Gyro bias: X: %fdps \t Y: %fdps \t Z: %fdps\r\n", bias.x, bias.y, bias.z);
            break;
        default: 
            print_readings(&bias);
        }
    }
}

/* Average 100 samples to find gyro bias */
void gyro_bias(struct gyro *bias) {
    if (!bias) {
        return;
    }

    bias->x = 0.0;
    bias->y = 0.0;
    bias->z = 0.0;

    int num = 0;
    while (num < 100) {
        acquire(&sensor_semaphore);

        if (current_sensor_readings.new_gyro) {
            current_sensor_readings.new_gyro = 0;

            bias->x += current_sensor_readings.gyro.x;
            bias->y += current_sensor_readings.gyro.y;
            bias->z += current_sensor_readings.gyro.z;

            num++;
        }

        release(&sensor_semaphore);

        usleep(1000);
    }

    bias->x /= 100;
    bias->y /= 100;
    bias->z /= 100;
}

void print_readings(struct gyro *bias) {
    struct sensors readings;

    acquire(&sensor_semaphore);
    memcpy(&readings, &current_sensor_readings, sizeof(struct sensors));
    release(&sensor_semaphore);

    /* Laulainen, E.; Koskinen, L.; Kosunen, M.; Halonen, K.; "Compass tilt compensation algorithm using CORDIC," Circuits and Systems, 2008. ISCAS 2008. IEEE International Symposium on , vol., no., pp.1188-1191, 18-21 May 2008
     * doi: 10.1109/ISCAS.2008.4541636 
     * The Z-axis is defined in the opposite direction, so those values are negated.
     * Equations 7 and 8 */
    float pitch = atan2(readings.accel.x, sqrtf(readings.accel.y*readings.accel.y + readings.accel.z*readings.accel.z));
    float roll = atan2(readings.accel.y, readings.accel.z);

    /* Equations 9, 10, 11 */
    float comp_x = readings.mag.x * cos(pitch) - readings.mag.y * sin(roll) * sin(pitch) - readings.mag.z * cos(roll) * sin(pitch);
    float comp_y = readings.mag.y * cos(roll) - readings.mag.z * sin(roll);
    float comp_heading;
    if (comp_y > 0) {
        comp_heading = 90.0 - atan(comp_x/comp_y) * RAD_TO_DEG;
    } else if (comp_y < 0) {
        comp_heading = 270.0 - atan(comp_x/comp_y) * RAD_TO_DEG;
    } else if (comp_y == 0 && comp_x < 0) {
        comp_heading = 180.0;
    } else {
        comp_heading = 0.0;
    }

    float uncomp_heading = atan2(readings.mag.y, readings.mag.x) * RAD_TO_DEG;
    uncomp_heading = uncomp_heading > 0 ? uncomp_heading : uncomp_heading + 360;

    float p,q,r = 0;
    /* The MPU6000 does LPF for us */
    p = readings.gyro.x;
    q = readings.gyro.y;
    r = readings.gyro.z;

    if (bias) {
        p -= bias->x;
        q -= bias->y;
        r -= bias->z;
    }

    printf("\r\nRaw data:\r\n");
    printf("Accel: \t X: %fg \t Y: %fg \t Z: %fg \r\n", readings.accel.x, readings.accel.y, readings.accel.z);
    printf("Gyro: \t X: %fdps \t Y: %fdps \t Z: %fdps \r\n", readings.gyro.x, readings.gyro.y, readings.gyro.z);
    printf("Mag: \t X: %fG \t Y: %fG \t Z: %fG \r\n", readings.mag.x, readings.mag.y, readings.mag.z);
    printf("Baro: %fmb \t Baro temp: %fC \t MPU temp: %fC \r\n", readings.baro.pressure, readings.baro.temp, readings.temp);
    printf("Computed data:\r\n");
    printf("Roll (phi): %fdeg \t Pitch (theta): %fdeg \r\n", roll*RAD_TO_DEG, pitch*RAD_TO_DEG);
    printf("Roll rate (p): %fdps \t Pitch rate (q): %fdps \t Yaw rate (r): %fdps \r\n", p, q, r);
    printf("Uncomp Heading: %fdeg \t Comp Heading: %fdeg \r\n", uncomp_heading, comp_heading);
}
