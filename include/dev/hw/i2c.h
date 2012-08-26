#ifndef DEV_HW_I2C_H_INCLUDED
#define DEV_HW_I2C_H_INCLUDED

struct semaphore;

extern struct semaphore i2c1_semaphore;

typedef struct i2c_dev {
    uint8_t (*read)(uint8_t);
    uint8_t (*write)(uint8_t, uint8_t*, uint32_t);
} i2c_dev;

void init_i2c1(void) __attribute__((section(".kernel")));

#endif
