void init_spi(void) __attribute__((section(".kernel")));
inline void spi_cs_high(void) __attribute((always_inline));
inline void spi_cs_low(void) __attribute__((always_inline));

inline void spi_cs_high(void) {
    *GPIOE_ODR |= (1 << 3);
}

inline void spi_cs_low(void) {
    *GPIOE_ODR &= ~(1 << 3);
}
