#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <kernel/semaphore.h>
#include <dev/sensors.h>

#include "matlab/codegen/lib/kalman/kalman.h"
#include "matlab/codegen/lib/kalman/kalman_initialize.h"
#include "matlab/codegen/lib/kalman/kalman_terminate.h"

#include "sensors.h"
#include "ui.h"

void kalman_filter(struct gyro *bias, real_T R[9]);
void calculate_R(real_T R[9]);
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

    real_T R[9] = {0.0};

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
        case 'k':
            kalman_filter(&bias, R);
            break;
        case 'r':
            printf("Finding R.  Do not move device!\r\n");
            calculate_R(R);
            printf("R (*10^-5):\t%f \t %f \t %f\r\n\t%f \t %f \t %f\r\n\t%f \t %f \t %f\r\n", R[0]*100000, R[1]*100000, R[2]*100000, R[3]*10000, R[4]*100000, R[5]*100000, R[6]*100000, R[7]*100000, R[8]*100000);
            break;
        default: 
            print_readings(&bias);
        }
    }
}

/* Beard, McLain. Navigation, Guidance, and Control of Small Unmanned Aircraft.
 * Section 8.5.2 */
void kalman_filter(struct gyro *bias, real_T R[9]) {
    printf("Kalman filtering roll and pitch.  Press any key to continue, reset to quit :)\r\n");
    getc();

    real_T N = 10.0;
    real_T T_out = 1.0;

    /* Process noise covariance */
    real_T Q[4] =   { 0, 0,
                      0, 0 };

    /* Measurement noise covariance */
    /*real_T R[9] =   { 1.0, 0.0, 0.0,
                      0.0, 1.0, 0.0,
                      0.0, 0.0, 1.0 };*/

    state state;
    state.Va = 0;
    state.g = 1;

    real_T roll_pitch[2];

    real_T accel[3];
    uint8_t new_accel = 0;

    float mag_x;
    float temp;

    kalman_initialize();

    while (1) {
        acquire(&sensor_semaphore);

        if (current_sensor_readings.new_gyro) {
            current_sensor_readings.new_gyro = 0;
            state.p = current_sensor_readings.gyro.x - bias->x;
            state.q = current_sensor_readings.gyro.y - bias->y;
            state.r = current_sensor_readings.gyro.z - bias->z;
        }
        if (current_sensor_readings.new_accel) {
            current_sensor_readings.new_accel = 0;
            new_accel = 1;
            accel[0] = current_sensor_readings.accel.x;
            accel[1] = current_sensor_readings.accel.y;
            accel[2] = current_sensor_readings.accel.z;
        }
        if (current_sensor_readings.new_mag) {
            current_sensor_readings.new_mag = 0;
            mag_x = current_sensor_readings.mag.x;
        }
        if (current_sensor_readings.new_baro) {
            current_sensor_readings.new_baro = 0;
            temp = current_sensor_readings.baro.temp;
        }

        release(&sensor_semaphore);

        if (new_accel) {
            new_accel = 0;
            kalman(state, 1, accel, N, T_out, R, Q, roll_pitch);
            //printf("New measurement! ");
        }
        else {
            kalman(state, 0, NULL, N, T_out, R, Q, roll_pitch);
        }

        printf("Roll: %fdeg \t Pitch: %fdeg \t Mag X: %f \t Baro temp: %f\r\n", roll_pitch[0]*RAD_TO_DEG, roll_pitch[1]*RAD_TO_DEG, mag_x, temp);
        usleep(10000);
    }

    kalman_terminate();
}

/* Improved Kalman Filter Method for Measurement Noise Reduction in Multi Sensor RFID Systems
 * Equations 16 and 17 */
void calculate_R(real_T R[9]) {
    if (!R) {
        return;
    }

    struct accelerometer avg = {};

    int num = 0;
    while (num < 100) {
        acquire(&sensor_semaphore);

        if (current_sensor_readings.new_accel) {
            current_sensor_readings.new_accel = 0;

            avg.x += current_sensor_readings.accel.x;
            avg.y += current_sensor_readings.accel.y;
            avg.z += current_sensor_readings.accel.z;

            num++;
        }

        release(&sensor_semaphore);

        usleep(1000);
    }

    avg.x /= 100;
    avg.y /= 100;
    avg.z /= 100;

    printf("Accel avg: X: %f \t Y: %f \t Z: %f\r\n", avg.x, avg.y, avg.z);

    R[0] = 0;
    R[1] = 0;
    R[2] = 0;
    R[3] = 0;
    R[4] = 0;
    R[5] = 0;
    R[6] = 0;
    R[7] = 0;
    R[8] = 0;

    num = 0;
    while (num < 100) {
        acquire(&sensor_semaphore);

        if (current_sensor_readings.new_accel) {
            current_sensor_readings.new_accel = 0;

            struct accelerometer diff = {
                .x = current_sensor_readings.accel.x - avg.x,
                .y = current_sensor_readings.accel.y - avg.y,
                .z = current_sensor_readings.accel.z - avg.z
            };

            R[0] += diff.x * diff.x;
            R[1] += diff.x * diff.y;
            R[2] += diff.x * diff.z;
            R[3] += diff.y * diff.x;
            R[4] += diff.y * diff.y;
            R[5] += diff.y * diff.z;
            R[6] += diff.z * diff.x;
            R[7] += diff.z * diff.y;
            R[8] += diff.z * diff.z;

            num++;
        }

        release(&sensor_semaphore);

        usleep(1000);
    }

    R[0] /= 99;
    R[1] /= 99;
    R[2] /= 99;
    R[3] /= 99;
    R[4] /= 99;
    R[5] /= 99;
    R[6] /= 99;
    R[7] /= 99;
    R[8] /= 99;
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
