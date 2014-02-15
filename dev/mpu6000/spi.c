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

#include <stdint.h>
#include <board_config.h>
#include <dev/device.h>
#include <dev/hw/gpio.h>
#include <dev/hw/spi.h>
#include <dev/mpu6000/class.h>
#include <kernel/init.h>
#include <kernel/obj.h>
#include <mm/mm.h>
#include "regs.h"

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

static int mpu6000_spi_probe(const char *name) {
    /* Check if the board has a valid config for the mpu6000. */
    return mpu6000_spi_config.valid == BOARD_CONFIG_VALID_MAGIC;
}

static struct obj *mpu6000_spi_ctor(const char *name) {
    struct obj *mpu6000_obj;
    struct mpu6000 *mpu;
    struct mpu6000_spi *mpu_spi;
    struct obj *cs_obj;
    struct gpio_ops *cs_ops;

    /* Check if the board has a valid config for the mpu6000. */
    if (mpu6000_spi_config.valid != BOARD_CONFIG_VALID_MAGIC) {
        return NULL;
    }

    /* Instantiate an mpu6000 obj with mpu6000_spi ops */
    mpu6000_obj = instantiate((char *)name, &mpu6000_class, &mpu6000_spi_ops,
                              struct mpu6000);
    if (!mpu6000_obj) {
        return NULL;
    }

    /* Connect mpu6000 to its parent SPI device */
    mpu = to_mpu6000(mpu6000_obj);
    mpu->device.parent = device_get(mpu6000_spi_config.parent_name);
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

    /* Get chip select GPIO */
    cs_obj = gpio_get(mpu6000_spi_config.cs_gpio);
    if (!cs_obj) {
        goto err_free_priv;
    }

    mpu_spi = (struct mpu6000_spi *) mpu->priv;
    mpu_spi->spi_dev.cs = to_gpio(cs_obj);
    mpu_spi->spi_dev.extended_transaction = 0;

    /* Initialize chip select */
    cs_ops = (struct gpio_ops *) cs_obj->ops;
    cs_ops->active_low(mpu_spi->spi_dev.cs,
                       mpu6000_spi_config.cs_active_low);
    cs_ops->direction(mpu_spi->spi_dev.cs, GPIO_OUTPUT);
    cs_ops->set_output_value(mpu_spi->spi_dev.cs, 1);

    /* Export to the OS */
    class_export_member(mpu6000_obj);

    return mpu6000_obj;

err_free_priv:
    kfree(mpu->priv);
err_free_obj:
    kfree(get_container(mpu6000_obj));
    return NULL;
}

static struct semaphore mpu6000_spi_driver_sem = INIT_SEMAPHORE;

static struct device_driver mpu6000_spi_driver = {
    .name = "mpu6000_spi",
    .probe = mpu6000_spi_probe,
    .ctor = mpu6000_spi_ctor,
    .class = &mpu6000_class,
    .sem = &mpu6000_spi_driver_sem,
};

static int mpu6000_spi_register(void) {
    device_driver_register(&mpu6000_spi_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_spi_register)
