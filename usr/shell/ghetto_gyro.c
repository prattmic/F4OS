#include <stdio.h>
#include <stdlib.h>
#include <dev/hw/i2c.h>
#include <arch/chip/dev/periph/9dof_gyro.h>
#include "app.h"

typedef struct gyro_data {
    short x;
    short y;
    short z;
} gyro_data;

/* Basically the same as accelerometer reading. Woohoo abstraction */
void ghetto_gyro(int argc, char **argv) {
    if(argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
    }

    gyro_data *data = malloc(sizeof(gyro_data));
    if (!data) {
        printf("Error: unable to allocate data.\r\n");
        return;
    }

    rd_t gyro_rd = open_sfe9dof_gyro();
    if (gyro_rd < 0) {
        printf("Error: unable to open gyro.\r\n");
        free(data);
        return;
    }

    printf("Press any key for data and q to quit\r\n");
    while (getc() != 'q') {
        if (read(gyro_rd, (char *)data, 6) == 6) {
            printf("X: %d   Y: %d   Z:%d\r\n", data->x, data->y, data->z);
        }
        else {
            printf("Unable to read gyro.\r\n");
        }
    }

    free(data);
    close(gyro_rd);
}
DEFINE_APP(ghetto_gyro)
