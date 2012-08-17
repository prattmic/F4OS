#include "shell_header.h"
#include "spi.h"
#include "discovery_accel.h"

typedef struct accel_data {
    short x;
    short y;
    short z;
} accel_data;

void accel(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }
    rd_t accelrd = open_discovery_accel();
    accel_data *data = malloc(sizeof(accel_data));
    printf("q to quit, any other key to get data.\r\n");
    while(1) {
        if(getc() == 'q') {
            return;
        }
        else {
            read(accelrd, (char *)data, 6);
            printf("X: %d Y: %d Z: %d\r\n", data->x, data->y, data->z);
        }
    }
}
