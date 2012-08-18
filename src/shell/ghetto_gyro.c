#include "shell_header.h"
#include "i2c.h"
#include "ghetto_gyro.h"

/* Remove/replace this with better IO */
void ghetto_gyro(int argc, char **argv) {
    uint8_t packet[2];

    /* Turn stuff on */
    packet[0] = 0x15;
    packet[1] = 0x07;
    i2c1_write(0x68, packet, 2);

    packet[0] = 0x16;
    packet[1] = 0x18;
    i2c1_write(0x68, packet, 2);

    printf("Press any key for data and q to quit\r\n");

    while (getc() != 'q') {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;

        packet[0] = 0x1D;
        if (!i2c1_write(0x68, packet, 1)) {
            x |= i2c1_read(0x68) << 8;
        }

        packet[0] = 0x1E;
        if (!i2c1_write(0x68, packet, 1)) {
            x |= i2c1_read(0x68);
        }

        packet[0] = 0x1F;
        if (!i2c1_write(0x68, packet, 1)) {
            y |= i2c1_read(0x68) << 8;
        }

        packet[0] = 0x20;
        if (!i2c1_write(0x68, packet, 1)) {
            y |= i2c1_read(0x68);
        }

        packet[0] = 0x21;
        if (!i2c1_write(0x68, packet, 1)) {
            z |= i2c1_read(0x68) << 8;
        }

        packet[0] = 0x22;
        if (!i2c1_write(0x68, packet, 1)) {
            z |= i2c1_read(0x68);
        }

        printf("X: %d   Y: %d   Z:%d\r\n", z,y,z);
    }
}
