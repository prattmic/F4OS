#include <stdlib.h>
#include <stdio.h>
#include <dev/hw/spi.h>
#include <dev/sensors.h>
#include <arch/chip/dev/periph/discovery_accel.h>
#include <math.h>
#include "app.h"

void accel(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    rd_t accelrd = open_discovery_accel();
    if (accelrd < 0) {
        printf("Error: unable to open accelerometer.\r\n");
        return;
    }

    struct accelerometer data;

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            close(accelrd);
            return;
        }
        else {
            if (!read_discovery_accel(accelrd, &data)) {
                printf("Roll: %f X: %f Y: %f Z: %f\r\n", atan2(data.z, data.y)*RAD_TO_DEG, data.x, data.y, data.z);
            }
            else {
                printf("Unable to read accelerometer.\r\n");
            }
        }
    }

    close(accelrd);
}
DEFINE_APP(accel)
