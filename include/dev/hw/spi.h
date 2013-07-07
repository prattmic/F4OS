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
    uint8_t extended_transaction;
} spi_dev;

extern struct spi_port spi1;

/**
 * Write data to SPI device.
 *
 * Write num bytes of data to SPI device dev on port spi
 *
 * @param spi   SPI port the device is connected to
 * @param dev   SPI device to write to
 * @param data  Data to write
 * @param num   Number of bytes of data to write
 *
 * @returns bytes written successfully, negative on error
 */
int spi_write(struct spi_port *spi, struct spi_dev *dev, uint8_t *data, uint32_t num);

/**
 * Read data from SPI device.
 *
 * Read num bytes of data from SPI device dev on port spi
 *
 * @param spi   SPI port the device is connected to
 * @param dev   SPI device to read from
 * @param data  Buffer to read data into
 * @param num   Number of bytes of data to read
 *
 * @returns bytes read into buffer, negative on error
 */
int spi_read(struct spi_port *spi, struct spi_dev *dev, uint8_t *data, uint32_t num);

/**
 * Begin extended SPI transaction
 *
 * spi_read and spi_write typically handle asserting the chip select
 * line themselves, however occasionally it is necessary to perform
 * multiple reads or writes with chip select asserted continuously.
 *
 * This function will assert the chip select line low, and mark an
 * extended transaction in progress.  During the extended transaction
 * spi_read and spi_write will not modify the state of the chip select
 * line.  Call spi_end_transaction when the extended transaction is complete.
 *
 * The SPI port should be held throughout an entire extended transaction,
 * meaning the application must not release control of the SPI port until
 * the extended transaction has completed.
 *
 * @param spi   SPI port the device is connected to
 * @param dev   SPI device to begin transaction with
 */
void spi_start_transaction(struct spi_port *spi, struct spi_dev *dev);

/**
 * End extended SPI transaction
 *
 * Set the device chip select line high and mark extended transaction as complete.
 * spi_read and spi_write will control the chip select line as normal.
 *
 * It is now safe to release control of the SPI port.
 *
 * @param spi   SPI port the device is connected to
 * @param dev   SPI device to begin transaction with
 */
void spi_end_transaction(struct spi_port *spi, struct spi_dev *dev);

#endif
