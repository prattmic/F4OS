#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <dev/hw/spi.h>
#include <dev/resource.h>
#include <kernel/class.h>
#include <kernel/collection.h>
#include <kernel/fault.h>
#include <kernel/init.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>
#include <board_config.h>

#include "lis302dl.h"

/*
 * STMicro LIS302DL Accelerometer Driver
 */

struct lis302dl {
    struct spi_dev spi_dev;
    int ready;
};

static int lis302dl_init(struct accel *a) {
    struct spi *spi = to_spi(a->device.parent);
    struct spi_ops *spi_ops = (struct spi_ops *)spi->obj.ops;
    struct lis302dl *lis_accel = (struct lis302dl *) a->priv;

    /* Active mode, XYZ enable */
    uint8_t data[2] = {
        LIS302DL_CTRL1,
        LIS302DL_CTRL1_PD | LIS302DL_CTRL1_XEN | LIS302DL_CTRL1_YEN | LIS302DL_CTRL1_ZEN,
    };
    if (spi_ops->write(spi, &lis_accel->spi_dev, data, 2) != 2) {
        /* Unable to activate :( */
        return -1;
    }

    return 0;
}

static int lis302dl_deinit(struct accel *a) {
    struct lis302dl *lis_accel = (struct lis302dl *) a->priv;
    int ret = 0;

    if (lis_accel->ready) {
        struct spi *spi = to_spi(a->device.parent);
        struct spi_ops *spi_ops = (struct spi_ops *)spi->obj.ops;

        /* Power down mode */
        uint8_t data[2] = {
            LIS302DL_CTRL1,
            0x00,
        };
        ret = spi_ops->write(spi, &lis_accel->spi_dev, data, 2);

        lis_accel->ready = 0;
    }

    return ret;
}

static int lis302dl_get_raw_data(struct accel *a, struct accel_raw_data *data) {
    struct spi *spi = to_spi(a->device.parent);
    struct spi_ops *spi_ops = (struct spi_ops *)spi->obj.ops;
    struct lis302dl *lis_accel = (struct lis302dl *) a->priv;

    /* Burst read of 5 bytes, starting at OUTX */
    uint8_t addr = LIS302DL_OUTX | ADDR_INC | SPI_READ;
    uint8_t databuf[5];

    if (!lis_accel->ready) {
        struct accel_ops *accel_ops = (struct accel_ops *)a->obj.ops;
        accel_ops->init(a);
    }

    spi_ops->start_transaction(spi, &lis_accel->spi_dev);

    /* Write start address */
    if (spi_ops->write(spi, &lis_accel->spi_dev, &addr, 1) != 1) {
        goto spi_err;
    }
    /* Read data */
    if (spi_ops->read(spi, &lis_accel->spi_dev, databuf, 5) != 5) {
        goto spi_err;
    }

    spi_ops->end_transaction(spi, &lis_accel->spi_dev);

    /* Ignore unused registers between output registers */
    data->x = (int8_t) databuf[0];
    data->y = (int8_t) databuf[2];
    data->z = (int8_t) databuf[4];

    return 0;

spi_err:
    spi_ops->end_transaction(spi, &lis_accel->spi_dev);
    return -1;
}

static int lis302dl_get_data(struct accel *a, struct accel_data *data) {
    struct accel_ops *accel_ops = (struct accel_ops *)a->obj.ops;
    struct accel_raw_data raw_data;
    int ret;

    ret = accel_ops->get_raw_data(a, &raw_data);
    if (ret) {
        return ret;
    }

    data->x = raw_data.x * LIS302DL_SENSITIVITY;
    data->y = raw_data.y * LIS302DL_SENSITIVITY;
    data->z = raw_data.z * LIS302DL_SENSITIVITY;

    return 0;
}

struct accel_ops lis302dl_ops = {
    .init = lis302dl_init,
    .deinit = lis302dl_deinit,
    .get_data = lis302dl_get_data,
    .get_raw_data = lis302dl_get_raw_data,
};

static int lis302dl_probe(const char *name) {
    /* Check if the board has a valid config for the accelerometer. */
    return lis302dl_accel_config.valid == BOARD_CONFIG_VALID_MAGIC;
}

static struct obj *lis302dl_ctor(const char *name) {
    struct obj *accel_obj;
    struct accel *accel;
    struct lis302dl *lis_accel;
    struct obj *cs_obj;
    struct gpio_ops *cs_ops;

    /* Check if the board has a valid config for the accelerometer.
     * There should be, this was checked in probe. */
    if (lis302dl_accel_config.valid != BOARD_CONFIG_VALID_MAGIC) {
        return NULL;
    }

    /* Instantiate an accel obj with lis302dl ops */
    accel_obj = instantiate((char *)name, &accel_class, &lis302dl_ops, struct accel);
    if (!accel_obj) {
        return NULL;
    }

    /* Connect accel to its parent SPI device */
    accel = to_accel(accel_obj);
    accel->device.parent = device_get(lis302dl_accel_config.parent_name);
    if (!accel->device.parent) {
        goto err_free_obj;
    }

    /* Set up private data */
    accel->priv = kmalloc(sizeof(struct lis302dl));
    if (!accel->priv) {
        goto err_free_obj;
    }

    lis_accel = (struct lis302dl *) accel->priv;
    lis_accel->ready = 0;
    lis_accel->spi_dev.extended_transaction = 0;

    /* Get chip select GPIO */
    cs_obj = gpio_get(lis302dl_accel_config.cs_gpio);
    if (!cs_obj) {
        goto err_free_priv;
    }

    lis_accel->spi_dev.cs = to_gpio(cs_obj);

    /* Initialize chip select */
    cs_ops = (struct gpio_ops *) cs_obj->ops;
    cs_ops->active_low(lis_accel->spi_dev.cs, lis302dl_accel_config.cs_active_low);
    cs_ops->direction(lis_accel->spi_dev.cs, GPIO_OUTPUT);
    cs_ops->set_output_value(lis_accel->spi_dev.cs, 1);

    /* Export to the OS */
    class_export_member(accel_obj);

    return accel_obj;

err_free_priv:
    kfree(accel->priv);
err_free_obj:
    kfree(accel_obj);
    return NULL;
}

/* Protects the constructor from reentrance */
static struct semaphore lis302dl_driver_sem = INIT_SEMAPHORE;

static struct device_driver lis320dl_driver = {
    .name = "lis302dl",
    .probe = lis302dl_probe,
    .ctor = lis302dl_ctor,
    .class = &accel_class,
    .sem = &lis302dl_driver_sem,
};

static int lis302dl_register(void) {
    device_driver_register(&lis320dl_driver);

    return 0;
}
CORE_INITIALIZER(lis302dl_register)
