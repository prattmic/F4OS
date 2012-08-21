typedef struct sfe9dof_gyro {
        i2c_dev *i2c_port;
        uint8_t device_addr;
        uint8_t tmp_addr;
        uint8_t addr_ctr;
} sfe9dof_gyro;

extern semaphore i2c1_semaphore;
extern i2c_dev i2c1;

rd_t open_sfe9dof_gyro(void) __attribute__((section(".kernel"))); 
char sfe9dof_gyro_read(void *env) __attribute__((section(".kernel"))); 
void sfe9dof_gyro_write(char d, void *env) __attribute__((section(".kernel"))); 
void sfe9dof_gyro_close(void *env) __attribute__((section(".kernel"))); 
