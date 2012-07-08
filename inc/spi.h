#define SPI_READ    (uint8_t) (1 << 7)

void init_spi(void) __attribute__((section(".kernel")));
uint8_t spi_write(uint8_t addr, uint8_t data) __attribute__((section(".kernel")));
uint8_t spi_read(uint8_t addr) __attribute__((section(".kernel")));
void accel_setup() __attribute__((section(".kernel")));
void accel_loop();

inline void spi_cs_high(void) __attribute((always_inline));
inline void spi_cs_low(void) __attribute__((always_inline));

inline void spi_cs_high(void) {
    *GPIOE_ODR |= (1 << 3);
}

inline void spi_cs_low(void) {
    *GPIOE_ODR &= ~(1 << 3);
}
