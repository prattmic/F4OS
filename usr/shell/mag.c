#include <stdio.h>
#include <dev/device.h>
#include <dev/mag.h>
#include <kernel/obj.h>
#include "app.h"

void mag(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("hmc5883");
    if (!o) {
        printf("Error: unable to find magnetometer.\r\n");
        return;
    }

    struct mag *mag = to_mag(o);
    struct mag_ops *ops = (struct mag_ops *) o->ops;
    struct mag_data data;

    printf("q to quit, any other key to get data.\r\nunits in gauss\r\n");

    while(1) {
        if(getc() == 'q') {
            return;
        }
        else {
            if (!ops->get_data(mag, &data)) {
                printf("X: %f Y: %f Z: %f\r\n", data.x, data.y, data.z);
            }
            else {
                printf("Unable to read magnetometer.\r\n");
            }
        }
    }
}
DEFINE_APP(mag)
