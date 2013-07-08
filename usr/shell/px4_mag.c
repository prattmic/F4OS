#include <stdio.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <arch/chip/dev/periph/px4_hmc5883.h>

#include "app.h"

void px4_mag(int argc, char **argv) {
    rd_t mag = open_px4_hmc5883();
    if (mag < 0) {
        printf("Error: unable to open magnetometer.\r\n");
        return;
    }

    printf("Press q to quit, any key to continue.\r\n");

    struct magnetometer mag_data;

    while (getc() != 'q') {
        if (!read_px4_hmc5883(mag, &mag_data)) {
            printf("X: %f \t Y: %f \t Z: %f \r\n", mag_data.x, mag_data.y, mag_data.z);
        }
        else {
            printf("Unable to read mag.\r\n");
        }
    }

    close(mag);
}
DEFINE_APP(px4_mag)
