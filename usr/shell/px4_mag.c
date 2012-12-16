#include <stdio.h>
#include <dev/resource.h>
#include <dev/periph/px4_hmc5883.h>

void px4_mag(int argc, char **argv) {
    rd_t mag = open_px4_hmc5883();

    printf("Press q to quit, any key to continue.\r\n");

    struct magnetometer mag_data;

    while (getc() != 'q') {
        read_px4_hmc5883(mag, &mag_data);

        printf("X: %f \t Y: %f \t Z: %f \r\n", mag_data.x, mag_data.y, mag_data.z);
    }

    close(mag);
}
