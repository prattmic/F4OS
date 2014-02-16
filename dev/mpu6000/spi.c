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

#include <libfdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <dev/device.h>
#include <dev/fdtparse.h>
#include <dev/hw/gpio.h>
#include <dev/hw/spi.h>
#include <dev/mpu6000/class.h>
#include <kernel/init.h>
#include <kernel/obj.h>
#include <mm/mm.h>
#include "regs.h"

#define MPU6000_SPI_COMPAT  "invensense,mpu6000-spi"

/* When set in register addres, perform a read instead of write */
#define MPU6000_SPI_READ    ((uint8_t) (1 << 7))

struct mpu6000_spi {
    struct spi_dev spi_dev;
};

/* Internal register write, which does not check ready bit */
static int _mpu6000_spi_write_reg(struct mpu6000 *mpu, uint8_t reg,
                                  uint8_t val) {
    struct spi *spi = to_spi(mpu->device.parent);
    struct spi_ops *spi_ops = (struct spi_ops *) spi->obj.ops;
    struct mpu6000_spi *mpu_spi = (struct mpu6000_spi *) mpu->priv;
    uint8_t packet[2] = {reg, val};

    if (spi_ops->write(spi, &mpu_spi->spi_dev, packet, 2) != 2) {
        return -1;
    }

    return 0;
}

static int mpu6000_spi_init(struct mpu6000 *mpu) {
    int ret;

    /*
     * Power management register 1
     * Active mode, clock with gyro X reference
     */
    ret = _mpu6000_spi_write_reg(mpu, MPU6000_PWR_MGMT_1,
                                 MPU6000_PWR_MGMT_1_CLK_PLLGYROX);

    if (ret) {
        return -1;
    }

    mpu->ready = 1;

    return 0;
}

static int mpu6000_spi_deinit(struct mpu6000 *mpu) {
    int ret;

    mpu->ready = 0;

    /*
     * Power management register 1
     * Sleep mode
     */
    ret = _mpu6000_spi_write_reg(mpu, MPU6000_PWR_MGMT_1,
                                 MPU6000_PWR_MGMT_1_SLEEP);

    if (ret) {
        return -1;
    }

    return 0;
}

static int mpu6000_spi_write_reg(struct mpu6000 *mpu, uint8_t reg,
                                 uint8_t val) {
    if (!mpu->ready) {
        struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
        mpu_ops->init(mpu);
    }

    return _mpu6000_spi_write_reg(mpu, reg, val);
}

static int mpu6000_spi_read_regs(struct mpu6000 *mpu, uint8_t reg,
                                 uint8_t *buf, uint8_t num) {
    struct spi *spi = to_spi(mpu->device.parent);
    struct spi_ops *spi_ops = (struct spi_ops *) spi->obj.ops;
    struct mpu6000_spi *mpu_spi = (struct mpu6000_spi *) mpu->priv;
    uint8_t read_reg = reg | MPU6000_SPI_READ;
    int ret = 0;

    if (!mpu->ready) {
        struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
        mpu_ops->init(mpu);
    }

    spi_ops->start_transaction(spi, &mpu_spi->spi_dev);

    /* Write start address */
    if (spi_ops->write(spi, &mpu_spi->spi_dev, &read_reg, 1) != 1) {
        goto out;
    }

    /* Read data */
    ret = spi_ops->read(spi, &mpu_spi->spi_dev, buf, num);

out:
    spi_ops->end_transaction(spi, &mpu_spi->spi_dev);

    return ret;
}

static int mpu6000_spi_enable_accel(struct mpu6000 *mpu, int enable) {
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    uint8_t reg_val;

    acquire(&mpu->lock);

    if (mpu_ops->read_regs(mpu, MPU6000_PWR_MGMT_2, &reg_val, 1) != 1) {
        goto err;
    }

    /* To enable accel, disable standby mode */
    if (enable) {
        reg_val &= ~(MPU6000_PWR_MGMT_2_STBY_XA |
                        MPU6000_PWR_MGMT_2_STBY_YA |
                        MPU6000_PWR_MGMT_2_STBY_ZA);
    }
    else {
        reg_val |= MPU6000_PWR_MGMT_2_STBY_XA |
                    MPU6000_PWR_MGMT_2_STBY_YA |
                    MPU6000_PWR_MGMT_2_STBY_ZA;
    }

    if (mpu_ops->write_reg(mpu, MPU6000_PWR_MGMT_2, reg_val)) {
        goto err;
    }

    if (enable) {
        mpu->accel_in_use = 1;
    }
    else {
        mpu->accel_in_use = 0;

        if (!mpu->gyro_in_use) {
            /* Go into low power mode */
            if (mpu_ops->deinit(mpu)) {
                goto err;
            }
        }
    }

    release(&mpu->lock);

    return 0;

err:
    release(&mpu->lock);
    return -1;
}

static int mpu6000_spi_enable_gyro(struct mpu6000 *mpu, int enable) {
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;

    acquire(&mpu->lock);

    if (enable) {
        mpu->gyro_in_use = 1;
    }
    else {
        /*
         * Don't actually disable the gyro, as it is used for the clock.
         * Just record that it isn't needed
         */
        mpu->gyro_in_use = 0;

        if (!mpu->accel_in_use) {
            /* Go into low power mode */
            if (mpu_ops->deinit(mpu)) {
                goto err;
            }
        }
    }

    release(&mpu->lock);

    return 0;

err:
    release(&mpu->lock);
    return -1;
}

struct mpu6000_ops mpu6000_spi_ops = {
    .init = mpu6000_spi_init,
    .deinit = mpu6000_spi_deinit,
    .write_reg = mpu6000_spi_write_reg,
    .read_regs = mpu6000_spi_read_regs,
    .enable_accelerometer = mpu6000_spi_enable_accel,
    .enable_gyroscope = mpu6000_spi_enable_gyro,
};

/*
 * Get and configure the CS GPIO for the MPU6000
 *
 * @param fdt   pointer to device tree blob
 * @param offset    mpu6000 offset in device tree
 * @returns pointer to configure CS GPIO, or NULL on error
 */
static struct gpio *mpu6000_spi_setup_cs(const void *fdt, int offset) {
    struct fdt_gpio cs_gpio;
    struct obj *cs_obj;
    struct gpio *cs;
    struct gpio_ops *cs_ops;
    int err;

    err = fdtparse_get_gpio(fdt, offset, "cs-gpio", &cs_gpio);
    if (err) {
        return NULL;
    }

    /* Get chip select GPIO */
    cs_obj = gpio_get(cs_gpio.gpio);
    if (!cs_obj) {
        return NULL;
    }

    cs = to_gpio(cs_obj);

    /* Initialize chip select */
    cs_ops = (struct gpio_ops *) cs_obj->ops;

    err = cs_ops->active_low(cs, cs_gpio.flags & GPIO_FDT_ACTIVE_LOW);
    if (err) {
        goto err_put_gpio;
    }

    err = cs_ops->direction(cs, GPIO_OUTPUT);
    if (err) {
        goto err_put_gpio;
    }

    err = cs_ops->set_output_value(cs, 1);
    if (err) {
        goto err_put_gpio;
    }

    return cs;

err_put_gpio:
    gpio_put(cs_obj);
    return NULL;
}

/* Verify that the WHOAMI register contains the correct value */
static int mpu6000_spi_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, ret;
    char *parent;
    struct obj *spi_obj;
    struct spi *spi;
    struct spi_ops *spi_ops;
    struct spi_dev spi_dev = {};

    /* Read of 1 byte, WHOAMI */
    uint8_t addr = MPU6000_WHOAMI | MPU6000_SPI_READ;
    uint8_t response;

    /* Default to assuming no device */
    ret = 0;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_SPI_COMPAT)) {
        return 0;
    }

    parent_offset = fdt_parent_offset(blob, offset);
    if (parent_offset < 0) {
        return 0;
    }

    parent = fdtparse_get_path(blob, parent_offset);
    if (!parent) {
        return 0;
    }

    spi_obj = device_get(parent);

    spi = to_spi(spi_obj);
    spi_ops = (struct spi_ops *)spi->obj.ops;

    spi_dev.cs = mpu6000_spi_setup_cs(blob, offset);
    if (!spi_dev.cs) {
        goto out_put_spi;
    }

    spi_ops->start_transaction(spi, &spi_dev);

    /* Write start address */
    if (spi_ops->write(spi, &spi_dev, &addr, 1) != 1) {
        goto out;
    }
    /* Read data */
    if (spi_ops->read(spi, &spi_dev, &response, 1) != 1) {
        goto out;
    }

    /*
     * Does the register contain the value it should?
     * Note: bits 0 and 7 of the register are reserved, don't check them.
     */
    ret = (response & 0x7E) == MPU6000_WHOAMI_VAL;

out:
    spi_ops->end_transaction(spi, &spi_dev);

    gpio_put(&spi_dev.cs->obj);
out_put_spi:
    device_put(spi_obj);
    free(parent);

    return ret;
}

static struct obj *mpu6000_spi_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset;
    char *parent;
    struct obj *mpu6000_obj;
    struct mpu6000 *mpu;
    struct mpu6000_spi *mpu_spi;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_SPI_COMPAT)) {
        return NULL;
    }

    parent_offset = fdt_parent_offset(blob, offset);
    if (parent_offset < 0) {
        return NULL;
    }

    parent = fdtparse_get_path(blob, parent_offset);
    if (!parent) {
        return NULL;
    }

    /* Instantiate an mpu6000 obj with mpu6000_spi ops */
    mpu6000_obj = instantiate(name, &mpu6000_class, &mpu6000_spi_ops,
                              struct mpu6000);
    if (!mpu6000_obj) {
        goto err_free_parent;
    }

    /* Connect mpu6000 to its parent SPI device */
    mpu = to_mpu6000(mpu6000_obj);
    mpu->device.parent = device_get(parent);
    if (!mpu->device.parent) {
        goto err_free_obj;
    }

    mpu->ready = 0;
    mpu->gyro_in_use = 0;
    mpu->accel_in_use = 0;
    init_semaphore(&mpu->lock);

    /* Set up private data */
    mpu->priv = kmalloc(sizeof(struct mpu6000_spi));
    if (!mpu->priv) {
        goto err_free_obj;
    }

    mpu_spi = (struct mpu6000_spi *) mpu->priv;
    mpu_spi->spi_dev.extended_transaction = 0;
    mpu_spi->spi_dev.cs = mpu6000_spi_setup_cs(blob, offset);
    if (!mpu_spi->spi_dev.cs) {
        goto err_free_priv;
    }

    /* Export to the OS */
    class_export_member(mpu6000_obj);

    free(parent);

    return mpu6000_obj;

err_free_priv:
    kfree(mpu->priv);
err_free_obj:
    kfree(get_container(mpu6000_obj));
err_free_parent:
    free(parent);
    return NULL;
}

static struct semaphore mpu6000_spi_driver_sem = INIT_SEMAPHORE;

static struct device_driver mpu6000_spi_compat_driver = {
    .name = MPU6000_SPI_COMPAT,
    .probe = mpu6000_spi_probe,
    .ctor = mpu6000_spi_ctor,
    .class = &mpu6000_class,
    .sem = &mpu6000_spi_driver_sem,
};

static int mpu6000_spi_register(void) {
    device_compat_driver_register(&mpu6000_spi_compat_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_spi_register)
