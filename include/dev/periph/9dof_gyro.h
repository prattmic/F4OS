#ifndef DEV_PERIPH_9DOF_GYRO_H_INCLUDED
#define DEV_PERIPH_9DOF_GYRO_H_INCLUDED

typedef struct sfe9dof_gyro {
        i2c_dev *i2c_port;
        uint8_t device_addr;
        uint8_t tmp_addr;
        uint8_t addr_ctr;
} sfe9dof_gyro;

extern struct semaphore i2c1_semaphore;
extern i2c_dev i2c1;

rd_t open_sfe9dof_gyro(void) __attribute__((section(".kernel"))); 

#endif
