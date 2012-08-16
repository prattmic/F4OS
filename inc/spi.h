#define SPI_READ    (uint8_t) (1 << 7)

typedef struct spi_dev {
    uint8_t write_addr;
    uint8_t write_ctr;
    void (*read)(uint8_t);
    void (*write)(uint8_t, uint8_t);
} spi_dev;

void init_spi(void) __attribute__((section(".kernel")));
uint8_t spi_write(uint8_t addr, uint8_t data) __attribute__((section(".kernel")));
uint8_t spi_read(uint8_t addr) __attribute__((section(".kernel")));
void discovery_accel_setup() __attribute__((section(".kernel")));
void accel_loop();

inline void spi_cs_high(void) __attribute((always_inline));
inline void spi_cs_low(void) __attribute__((always_inline));

inline void spi_cs_high(void) {
    *GPIOE_ODR |= (1 << 3);
}

inline void spi_cs_low(void) {
    *GPIOE_ODR &= ~(1 << 3);
}
