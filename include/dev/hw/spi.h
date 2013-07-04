#ifndef DEV_HW_SPI_H_INCLUDED
#define DEV_HW_SPI_H_INCLUDED

struct semaphore;

extern struct semaphore spi1_semaphore;

struct spi;

struct spi_port {
    uint8_t     ready;
    struct spi  *regs;
    void        (*init)(void);
} spi_port;

struct spi_dev {
    void    (*cs_high)(void);
    void    (*cs_low)(void);
} spi_dev;

extern struct spi_port spi1;

int spi_write(struct spi_port *spi, struct spi_dev *dev, uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));
int spi_read(struct spi_port *spi, struct spi_dev *dev, uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));

#endif
