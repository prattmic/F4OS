#include <stdio.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <arch/chip/dev/periph/px4_ms5611.h>

#include "app.h"

void px4_baro(int argc, char **argv) {
    rd_t baro = open_px4_ms5611();
    if (baro < 0) {
        printf("Error: unable to open barometer.\r\n");
        return;
    }

    printf("Press q to quit, any key to continue.\r\n");

    while (getc() != 'q') {
        struct barometer data;

        if(!read_px4_ms5611(baro, &data)) {
            printf("%f mbar at %f C\r\n", data.pressure, data.temp);
        }
        else {
            printf("Unable to read baro.\r\n");
        }
    }

    close(baro);
}
DEFINE_APP(px4_baro)
