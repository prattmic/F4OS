#include <stdio.h>
#include <dev/device.h>
#include <dev/baro.h>
#include <kernel/obj.h>
#include "app.h"

void baro(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("ms5611");
    if (!o) {
        printf("Error: unable to find barometer.\r\n");
        return;
    }

    struct baro *baro = to_baro(o);
    struct baro_ops *ops = (struct baro_ops *) o->ops;
    struct baro_data data;
    uint8_t has_temp = ops->has_temp(baro);

    printf("Baro provides temp: %s\r\n\r\n", has_temp ? "Yes" : "No");

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(baro, &data)) {
                if (has_temp) {
                    printf("Temp: %fC\t", data.temperature);
                }

                printf("Pressure: %fPa\r\n", data.pressure);
            }
            else {
                printf("Unable to read barometer.\r\n");
            }
        }
    }
}
DEFINE_APP(baro)
