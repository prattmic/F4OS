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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <dev/fdtparse.h>
#include <dev/hw/spi.h>
#include <dev/resource.h>
#include <kernel/class.h>
#include <kernel/collection.h>
#include <kernel/fault.h>
#include <kernel/init.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#include "lis302dl.h"

/*
 * STMicro LIS302DL Accelerometer Driver
 */

#define LIS302DL_COMPAT "stmicro,lis302dl"

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

/*
 * Get and configure the CS GPIO for the LIS302DL
 *
 * @param fdt   pointer to device tree blob
 * @param offset    lis320dl offset in device tree
 * @returns pointer to configure CS GPIO, or NULL on error
 */
static struct gpio *lis302dl_setup_cs(const void *fdt, int offset) {
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
static int lis302dl_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, ret;
    char *parent;
    struct obj *spi_obj;
    struct spi *spi;
    struct spi_ops *spi_ops;
    struct spi_dev spi_dev = {};

    /* Read of 1 byte, WHOAMI */
    uint8_t addr = LIS302DL_WHOAMI | SPI_READ;
    uint8_t response;

    /* Default to assuming no device */
    ret = 0;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, LIS302DL_COMPAT)) {
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

    spi_dev.cs = lis302dl_setup_cs(blob, offset);
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

    /* Does the register contain the value it should? */
    ret = response == LIS302DL_WHOAMI_VAL;

out:
    spi_ops->end_transaction(spi, &spi_dev);

    gpio_put(&spi_dev.cs->obj);
out_put_spi:
    device_put(spi_obj);
    free(parent);

    return ret;
}

static struct obj *lis302dl_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset;
    char *parent;
    struct obj *accel_obj;
    struct accel *accel;
    struct lis302dl *lis_accel;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, LIS302DL_COMPAT)) {
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

    /* Instantiate an accel obj with lis302dl ops */
    accel_obj = instantiate(name, &accel_class, &lis302dl_ops, struct accel);
    if (!accel_obj) {
        goto err_free_parent;
    }

    /* Connect accel to its parent SPI device */
    accel = to_accel(accel_obj);
    accel->device.parent = device_get(parent);
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

    lis_accel->spi_dev.cs = lis302dl_setup_cs(blob, offset);
    if (!lis_accel->spi_dev.cs) {
        goto err_free_priv;
    }

    /* Export to the OS */
    class_export_member(accel_obj);

    free(parent);

    return accel_obj;

err_free_priv:
    kfree(accel->priv);
err_free_obj:
    kfree(get_container(accel_obj));
err_free_parent:
    free(parent);
    return NULL;
}

/* Protects the constructor from reentrance */
static struct semaphore lis302dl_driver_sem = INIT_SEMAPHORE;

static struct device_driver lis302dl_compat_driver = {
    .name = LIS302DL_COMPAT,
    .probe = lis302dl_probe,
    .ctor = lis302dl_ctor,
    .class = &accel_class,
    .sem = &lis302dl_driver_sem,
};

static int lis302dl_register(void) {
    device_compat_driver_register(&lis302dl_compat_driver);

    return 0;
}
CORE_INITIALIZER(lis302dl_register)
