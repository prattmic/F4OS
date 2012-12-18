#ifndef DEV_HW_I2C_H_INCLUDED
#define DEV_HW_I2C_H_INCLUDED

typedef struct i2c_dev {
    uint8_t ready;
    uint8_t port;
    void    (*init)(void);
} i2c_dev;

int8_t i2c_write(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num);
int i2c_read(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num);

extern i2c_dev i2c1;
extern i2c_dev i2c2;

struct semaphore;
extern struct semaphore i2c1_semaphore;
extern struct semaphore i2c2_semaphore;

#endif
