/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEV_HW_SPI_H_INCLUDED
#define DEV_HW_SPI_H_INCLUDED

#include <dev/hw/gpio.h>
#include <kernel/obj.h>
#include <kernel/mutex.h>

struct spi {
    struct mutex        lock;
    void                *priv;
    struct obj          obj;
};

struct spi_dev {
    /*
     * Should be configure by child driver such that setting the GPIO to 0
     * activates the chip select.
     * */
    struct gpio *cs;
    uint8_t extended_transaction;
};

/* Takes obj and returns containing struct spi */
static inline struct spi *to_spi(struct obj *o) {
    return (struct spi *) container_of(o, struct spi, obj);
}

struct spi_ops {
    /**
     * Initialize SPI peripheral
     *
     * Initialize SPI peripheral to be ready for communication.  Sets
     * up SPI hardware registers or software emulation, and prepares
     * any internal data structures.  Returns success if peripheral
     * is already initialized.
     *
     * Calling this function is not required.  The peripheral will be
     * lazily initialized on first use.
     *
     * @param spi   SPI peripheral to initialize
     *
     * @returns zero on success, negative on error
     */
    int         (*init)(struct spi *);
    /**
     * Deinitialize SPI peripheral
     *
     * Frees any internal data structures and optionally powers down
     * hardware peripherals.
     *
     * Calling this function on a non-initialized peripheral has no effect.
     *
     * @param spi   SPI peripheral to deinitialize
     *
     * @returns zero on success, negative on error
     */
    int         (*deinit)(struct spi *);
    /**
     * Read and write data to and from SPI device.
     *
     * Read and write num bytes of data to SPI device dev on port spi,
     * setting the device chip select automatically.
     *
     * Either read_data to write_data may be NULL, but if non-NULL, each
     * must be able to store at least num bytes.
     *
     * read_data[i] will contain data read in the same clock that
     * write_data[i] was written.
     *
     * @param spi           SPI port the device is connected to
     * @param dev           SPI device to write to
     * @param read_data     Buffer to write data read to
     * @param write_data    Data to write
     * @param num           Number of bytes of data to read/write
     *
     * @returns bytes read/written successfully, negative on error
     */
    int         (*read_write)(struct spi *, struct spi_dev *, uint8_t *, uint8_t *, uint32_t);
    /**
     * Write data to SPI device.
     *
     * Write num bytes of data to SPI device dev on port spi, setting
     * the device chip select automatically.
     *
     * @param spi   SPI port the device is connected to
     * @param dev   SPI device to write to
     * @param data  Data to write
     * @param num   Number of bytes of data to write
     *
     * @returns bytes written successfully, negative on error
     */
    int         (*write)(struct spi *, struct spi_dev *, uint8_t *, uint32_t);
    /**
     * Read data from SPI device.
     *
     * Read num bytes of data from SPI device dev on port spi, setting
     * the device chip select automatically.
     *
     * @param spi   SPI port the device is connected to
     * @param dev   SPI device to read from
     * @param data  Buffer to read data into
     * @param num   Number of bytes of data to read
     *
     * @returns bytes read into buffer, negative on error
     */
    int         (*read)(struct spi *, struct spi_dev *, uint8_t *, uint32_t);
    /**
     * Begin extended SPI transaction
     *
     * Begin a transaction that lasts for multiple calls.  Until end_transaction
     * is called, the chip select line will be held low, and any peripheral
     * specific locks will be held.
     *
     * end_transaction must be called when the transaction is complete.
     *
     * @param spi   SPI port the device is connected to
     * @param dev   SPI device to begin transaction with
     */
    void        (*start_transaction)(struct spi *, struct spi_dev *);
    /**
     * End extended SPI transaction
     *
     * Release any peripheral locks and set the chip select line high.
     *
     * This must only be called after start_transaction.
     *
     * @param spi   SPI port the device is connected to
     * @param dev   SPI device to begin transaction with
     */
    void        (*end_transaction)(struct spi *, struct spi_dev *);
};

extern struct class spi_class;
#endif
