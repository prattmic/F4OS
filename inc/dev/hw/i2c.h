void init_i2c1(void) __attribute__((section(".kernel")));
void i2c1_write(uint8_t addr, uint8_t data) __attribute__((section(".kernel")));
uint8_t i2c1_read(uint8_t addr) __attribute__((section(".kernel")));

inline void i2c1_stop(void) __attribute__((always_inline));
inline void i2c1_stop(void) {
    *I2C1_CR1 |= I2C_CR1_STOP;
}
