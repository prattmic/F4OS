#ifndef DEV_HW_SPI_H_INCLUDED
#define DEV_HW_SPI_H_INCLUDED

struct semaphore;

extern struct semaphore spi1_semaphore;

typedef struct spi_dev {
    uint8_t curr_addr;
    uint8_t addr_ctr;
    uint8_t (*read)(uint8_t, void(*)(void), void(*)(void));
    uint8_t (*write)(uint8_t, uint8_t, void(*)(void), void(*)(void));
} spi_dev;

void init_spi(void) __attribute__((section(".kernel")));

#endif
