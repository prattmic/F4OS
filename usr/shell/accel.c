#include <stdlib.h>
#include <stdio.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <kernel/obj.h>
#include <math.h>
#include "app.h"

void accel(int argc, char **argv) {
    const char *driver;

    if (argc != 1 && argc != 2) {
        printf("Usage: %s [driver]\r\n", argv[0]);
        return;
    }

    if (argc == 2) {
        driver = argv[1];
    }
    else {
        int total;

        /* Hopefully, there is only one accelerometer driver */
        total = device_list_class(&accel_class, &driver, 1);
        if (total < 1) {
            printf("Error: No accelerometer drivers found.\r\n");
            return;
        }
        else if (total > 1) {
            const char *names[total];
            /* Now, get all of the drivers */
            total = device_list_class(&accel_class, names, total);

            printf("Multiple accelerometer drivers found:\r\n");
            for (int i = 0; i < total; i++) {
                printf("* %s\r\n", names[i]);
            }

            printf("Please specify a driver as an argument\r\n");

            return;
        }
    }

    printf("Connecting to accelerometer '%s'\r\n", driver);

    struct obj *o = device_get(driver);
    if (!o) {
        printf("Error: unable to find accelerometer.\r\n");
        return;
    }

    struct accel *a = to_accel(o);
    struct accel_ops *ops = (struct accel_ops *) o->ops;
    struct accel_data data;

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
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
