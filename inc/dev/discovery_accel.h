
typedef struct discovery_accel {
        spi_dev *spi_port;
        uint8_t read_ctr;
} discovery_accel;

char discovery_accel_read(void *env) __attribute__((section(".kernel")));
void discovery_accel_write(char d, void *env) __attribute__((section(".kernel")));
rd_t open_discovery_accel(void) __attribute__((section(".kernel")));
