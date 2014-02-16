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
#include <dev/fdtparse.h>
#include <dev/hw/i2c.h>
#include <dev/gyro.h>
#include <kernel/init.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#define ITG3200_COMPAT  "invensense,itg3200"

#define ITG3200_WHOAMI      0x0     /* Who Am I register */
/* Who Am I register value (bits 6:1 defined) */
#define ITG3200_WHOAMI_VAL  0x68

#define ITG3200_XOUT_H  0x1D    /* X output high data register */
#define ITG3200_GAIN    14.375  /* LSB/(deg/s) */

struct itg3200 {
    uint8_t             ready;
    int                 addr;
    struct semaphore    lock;
};

static int itg3200_init(struct gyro *gyro) {
    struct i2c *i2c = to_i2c(gyro->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct itg3200 *itg_gyro = (struct itg3200 *) gyro->priv;
    uint8_t packet[3];
    int ret;

    acquire(&itg_gyro->lock);

    packet[0] = 0x3E;   /* Power Management Register */
    packet[1] = 0x01;   /* Clock PLL with X gyro reference */
    ret = i2c_ops->write(i2c, itg_gyro->addr, packet, 2);
    if (ret != 2) {
        goto err;
    }

    packet[0] = 0x15;   /* Sample Rate Divider Register */
    packet[1] = 0x07;   /* Sample rate = 1kHz / (7 + 1) = 125Hz */
    packet[2] = 0x18;   /* (Full Scale Register) Gyro full-scale range */
    ret = i2c_ops->write(i2c, itg_gyro->addr, packet, 3);
    if (ret != 3) {
        goto err;
    }

    release(&itg_gyro->lock);

    itg_gyro->ready = 1;

    return 0;

err:
    release(&itg_gyro->lock);
    return -1;
}

static int itg3200_deinit(struct gyro *gyro) {
    struct i2c *i2c = to_i2c(gyro->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct itg3200 *itg_gyro = (struct itg3200 *) gyro->priv;
    uint8_t packet[2];
    int ret;

    itg_gyro->ready = 0;

    acquire(&itg_gyro->lock);

    packet[0] = 0x3E;   /* Power Management Register */
    packet[1] = 0x40;   /* Sleep mode */
    ret = i2c_ops->write(i2c, itg_gyro->addr, packet, 2);

    release(&itg_gyro->lock);

    if (ret != 2) {
        return -1;
    }

    return 0;
}

static int itg3200_get_raw_data(struct gyro *gyro, struct gyro_raw_data *data) {
    struct i2c *i2c = to_i2c(gyro->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct itg3200 *itg_gyro = (struct itg3200 *) gyro->priv;
    uint8_t raw_data[6];
    int ret;

    if (!itg_gyro->ready) {
        struct gyro_ops *gyro_ops = (struct gyro_ops *) gyro->obj.ops;
        gyro_ops->init(gyro);
    }

    acquire(&itg_gyro->lock);

    /* Start reading from XOUT_H */
    raw_data[0] = ITG3200_XOUT_H;
    ret = i2c_ops->write(i2c, itg_gyro->addr, raw_data, 1);
    if (ret != 1) {
        goto out_err;
    }

    /* Read the six data registers */
    ret = i2c_ops->read(i2c, itg_gyro->addr, raw_data, 6);
    if (ret != 6) {
        goto out_err;
    }

    release(&itg_gyro->lock);

    data->x = (int16_t) ((raw_data[0] << 8) | raw_data[1]);
    data->z = (int16_t) ((raw_data[2] << 8) | raw_data[3]);
    data->y = (int16_t) ((raw_data[4] << 8) | raw_data[5]);

    return 0;

out_err:
    release(&itg_gyro->lock);
    return -1;
}

static int itg3200_get_data(struct gyro *gyro, struct gyro_data *data) {
    struct gyro_ops *gyro_ops = (struct gyro_ops *) gyro->obj.ops;
    struct gyro_raw_data raw_data;
    int ret;

    ret = gyro_ops->get_raw_data(gyro, &raw_data);
    if (ret) {
        return ret;
    }

    data->x = ((float) raw_data.x)/ITG3200_GAIN;
    data->y = ((float) raw_data.y)/ITG3200_GAIN;
    data->z = ((float) raw_data.z)/ITG3200_GAIN;

    return 0;
}

struct gyro_ops itg3200_ops = {
    .init = itg3200_init,
    .deinit = itg3200_deinit,
    .get_data = itg3200_get_data,
    .get_raw_data = itg3200_get_raw_data,
};

/* Verify contents of WHOAMI register */
static int itg3200_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, addr, err;
    char *parent;
    struct obj *i2c_obj;
    struct i2c *i2c;
    struct i2c_ops *i2c_ops;
    uint8_t data;
    int ret = 0;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, ITG3200_COMPAT)) {
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

    i2c_obj = device_get(parent);
    if (!i2c_obj) {
        goto out_free_parent;
    }

    i2c = to_i2c(i2c_obj);
    i2c_ops = (struct i2c_ops *)i2c->obj.ops;

    err = fdtparse_get_int(blob, offset, "reg", &addr);
    if (err) {
        goto out;
    }

    /* Attempt read of WHOAMI register */
    data = ITG3200_WHOAMI;
    err = i2c_ops->write(i2c, addr, &data, 1);
    if (err != 1) {
        ret = 0;
        goto out;
    }

    err = i2c_ops->read(i2c, addr, &data, 1);
    if (err != 1) {
        ret = 0;
        goto out;
    }

    /*
     * The WHOAMI register contains bits 6:1 of the I2C device address.
     * Verify it matches the value from the datasheet.
     */
    ret = (data & 0x7e) == ITG3200_WHOAMI_VAL;

out:
    device_put(i2c_obj);
out_free_parent:
    free(parent);
    return ret;
}

static struct obj *itg3200_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, err;
    char *parent;
    struct obj *gyro_obj;
    struct gyro *gyro;
    struct itg3200 *itg_gyro;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, ITG3200_COMPAT)) {
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

    /* Instantiate an gyro obj with itg3200 ops */
    gyro_obj = instantiate(name, &gyro_class, &itg3200_ops, struct gyro);
    if (!gyro_obj) {
        goto err_free_parent;
    }

    /* Connect gyro to its parent I2C device */
    gyro = to_gyro(gyro_obj);
    gyro->device.parent = device_get(parent);
    if (!gyro->device.parent) {
        goto err_free_obj;
    }

    /* Set up private data */
    gyro->priv = kmalloc(sizeof(struct itg3200));
    if (!gyro->priv) {
        goto err_free_obj;
    }

    itg_gyro = (struct itg3200 *) gyro->priv;
    itg_gyro->ready = 0;
    init_semaphore(&itg_gyro->lock);

    err = fdtparse_get_int(blob, offset, "reg", &itg_gyro->addr);
    if (err) {
        goto err_free_priv;
    }

    /* Export to the OS */
    class_export_member(gyro_obj);

    free(parent);

    return gyro_obj;

err_free_priv:
    kfree(gyro->priv);
err_free_obj:
    kfree(get_container(gyro_obj));
err_free_parent:
    free(parent);
    return NULL;
}

static struct semaphore itg3200_driver_sem = INIT_SEMAPHORE;

static struct device_driver itg3200_compat_driver = {
    .name = ITG3200_COMPAT,
    .probe = itg3200_probe,
    .ctor = itg3200_ctor,
    .class = &gyro_class,
    .sem = &itg3200_driver_sem,
};

static int itg3200_register(void) {
    device_compat_driver_register(&itg3200_compat_driver);

    return 0;
}
CORE_INITIALIZER(itg3200_register)
