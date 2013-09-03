#include <stdlib.h>
#include <stdio.h>
#include <dev/device.h>
#include <dev/gyro.h>
#include <kernel/obj.h>
#include "app.h"

void gyro(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("mpu6000_gyro");
    if (!o) {
        printf("Error: unable to find gyroscope.\r\n");
        return;
    }

    struct gyro *gyro = to_gyro(o);
    struct gyro_ops *ops = (struct gyro_ops *) o->ops;
    struct gyro_data data;

    printf("q to quit, any other key to get data.\r\nunits in deg/s\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(gyro, &data)) {
                printf("X: %f Y: %f Z: %f\r\n", data.x, data.y, data.z);
            }
            else {
                printf("Unable to read gyroscope.\r\n");
            }
        }
    }
}
DEFINE_APP(gyro)
