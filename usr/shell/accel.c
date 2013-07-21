#include <stdlib.h>
#include <stdio.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <kernel/obj.h>
#include <math.h>
#include "app.h"

void accel(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("lis302dl");
    if (!o) {
        printf("Error: unable to find accelerometer.\r\n");
        return;
    }

    struct accel *a = (struct accel *) to_accel(o);
    struct accel_ops *ops = (struct accel_ops *) o->ops;
    struct accel_data data;

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            return;
        }
        else {
            if (!ops->get_data(a, &data)) {
                printf("Roll: %f X: %f Y: %f Z: %f\r\n", atan2(data.z, data.y)*RAD_TO_DEG, data.x, data.y, data.z);
            }
            else {
                printf("Unable to read accelerometer.\r\n");
            }
        }
    }
}
DEFINE_APP(accel)
