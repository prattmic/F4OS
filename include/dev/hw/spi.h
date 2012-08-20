#define SPI_READ    (uint8_t) (1 << 7)

typedef struct spi_dev {
    uint8_t curr_addr;
    uint8_t addr_ctr;
    uint8_t (*read)(uint8_t);
    uint8_t (*write)(uint8_t, uint8_t);
} spi_dev;

uint8_t spinowrite(uint8_t addr, uint8_t data) __attribute__((section(".kernel")));
uint8_t spinoread(uint8_t addr) __attribute__((section(".kernel")));


void init_spi1(void) __attribute__((section(".kernel")));
uint8_t spi1_write(uint8_t addr, uint8_t data) __attribute__((section(".kernel")));
uint8_t spi1_read(uint8_t addr) __attribute__((section(".kernel")));
void discovery_accel_setup() __attribute__((section(".kernel")));

inline void spi1_cs_high(void) __attribute__((always_inline));
inline void spi1_cs_low(void) __attribute__((always_inline));

inline void spi1_cs_high(void) {
    *GPIOE_ODR |= (1 << 3);
}

inline void spi1_cs_low(void) {
    *GPIOE_ODR &= ~(1 << 3);
}
