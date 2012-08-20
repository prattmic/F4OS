typedef struct i2c_dev {
    uint8_t read_ctr;
    uint8_t write_ctr;
    uint8_t read_addr;
    uint8_t write_addr;
    uint8_t (*read)(uint8_t);
    uint8_t (*write)(uint8_t, uint8_t*, uint32_t);
} i2c_dev;

void init_i2c1(void) __attribute__((section(".kernel")));
uint8_t i2c1_write(uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));
uint8_t i2c1_read(uint8_t addr) __attribute__((section(".kernel")));
uint8_t i2cnowrite(uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));
uint8_t i2cnoread(uint8_t addr) __attribute__((section(".kernel")));

void i2c1_stop(void) __attribute__((section(".kernel")));
