#include <stdio.h>
#include <stdlib.h>

#include <dev/hw/i2c.h>
#include <dev/periph/9dof_gyro.h>

#include "ghetto_gyro.h"

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
    rd_t gyro_rd = open_sfe9dof_gyro();
    printf("Press any key for data and q to quit\r\n");
    while (getc() != 'q') {
        read(gyro_rd, (char *)data, 6);
        printf("X: %d   Y: %d   Z:%d\r\n", data->x, data->y, data->z);
    }
    free(data);
    close(gyro_rd);
}
