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
#include <dev/fdtparse.h>
#include <dev/hw/i2c.h>
#include <dev/mag.h>
#include <kernel/init.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#define HMC5883_COMPAT  "honeywell,hmc5883"
#define HMC5883L_COMPAT  "honeywell,hmc5883l"

#define HMC5883_GAIN    1024
#define HMC5883L_GAIN   1090
#define HMC5883_OUTXA   0x03    /* First output register */
#define HMC5883_IDA     0x0A    /* First ID register */

#define HMC5883_IDA_VAL 0x48    /* IDA register value */
#define HMC5883_IDB_VAL 0x34    /* IDB register value */
#define HMC5883_IDC_VAL 0x33    /* IDC register value */

struct hmc5883 {
    uint8_t             ready;
    int                 addr;
    /* Differentiate between HMC5883 and HMC5883L */
    uint8_t             is_hmc5883l;
    struct semaphore    lock;
};

static int hmc5883_init(struct mag *mag) {
    struct i2c *i2c = to_i2c(mag->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct hmc5883 *hmc_mag = (struct hmc5883 *) mag->priv;
    uint8_t packet[4];
    int ret;

    acquire(&hmc_mag->lock);

    packet[0] = 0x00;   /* Configuration register A */
    packet[1] = 0x18;   /* 75 Hz output, Normal measurement mode */
    if (hmc_mag->is_hmc5883l) {
        /* Averaged samples only supported on HMC5883L */
        packet[1] |= 0x60;  /* 8 samples/output */
    }
    packet[2] = 0x20;   /* auto-increment to config reg B; gain to 1090 */
    packet[3] = 0x00;   /* auto-increment to mode reg; continuous mode */
    ret = i2c_ops->write(i2c, hmc_mag->addr, packet, 4);

    release(&hmc_mag->lock);

    if (ret != 4) {
        return -1;
    }

    hmc_mag->ready = 1;

    return 0;
}

static int hmc5883_deinit(struct mag *mag) {
    struct i2c *i2c = to_i2c(mag->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct hmc5883 *hmc_mag = (struct hmc5883 *) mag->priv;
    uint8_t packet[2];
    int ret;

    hmc_mag->ready = 0;

    acquire(&hmc_mag->lock);

    packet[0] = 0x02;   /* Mode register */
    packet[1] = 0x01;   /* Idle mode */
    ret = i2c_ops->write(i2c, hmc_mag->addr, packet, 2);

    release(&hmc_mag->lock);

    if (ret != 2) {
        return -1;
    }

    return 0;
}

static int hmc5883_get_raw_data(struct mag *mag, struct mag_raw_data *data) {
    struct i2c *i2c = to_i2c(mag->device.parent);
    struct i2c_ops *i2c_ops = (struct i2c_ops *)i2c->obj.ops;
    struct hmc5883 *hmc_mag = (struct hmc5883 *) mag->priv;
    uint8_t raw_data[6];
    int ret;

    if (!hmc_mag->ready) {
        struct mag_ops *mag_ops = (struct mag_ops *) mag->obj.ops;
        mag_ops->init(mag);
    }

    acquire(&hmc_mag->lock);

    /* Start reading from OUTXA */
    raw_data[0] = HMC5883_OUTXA;
    ret = i2c_ops->write(i2c, hmc_mag->addr, raw_data, 1);
    if (ret != 1) {
        goto out_err;
    }

    /* Read the six data registers */
    ret = i2c_ops->read(i2c, hmc_mag->addr, raw_data, 6);
    if (ret != 6) {
        goto out_err;
    }

    release(&hmc_mag->lock);

    data->x = (int16_t) ((raw_data[0] << 8) | raw_data[1]);
    data->z = (int16_t) ((raw_data[2] << 8) | raw_data[3]);
    data->y = (int16_t) ((raw_data[4] << 8) | raw_data[5]);

    return 0;

out_err:
    release(&hmc_mag->lock);
    return -1;
}

static int hmc5883_get_data(struct mag *mag, struct mag_data *data) {
    struct mag_ops *mag_ops = (struct mag_ops *) mag->obj.ops;
    struct hmc5883 *hmc_mag = (struct hmc5883 *) mag->priv;
    struct mag_raw_data raw_data;
    int ret, divisor;

    ret = mag_ops->get_raw_data(mag, &raw_data);
    if (ret) {
        return ret;
    }

    if (hmc_mag->is_hmc5883l) {
        divisor = HMC5883L_GAIN;
    }
    else {
        divisor = HMC5883_GAIN;
    }

    data->x = ((float) raw_data.x)/divisor;
    data->y = ((float) raw_data.y)/divisor;
    data->z = ((float) raw_data.z)/divisor;

    return 0;
}

struct mag_ops hmc5883_ops = {
    .init = hmc5883_init,
    .deinit = hmc5883_deinit,
    .get_data = hmc5883_get_data,
    .get_raw_data = hmc5883_get_raw_data,
};

/* Identify chip by verifying the contents of the 3 ID registers */
static int hmc5883_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, addr, err;
    char *parent;
    struct obj *i2c_obj;
    struct i2c *i2c;
    struct i2c_ops *i2c_ops;
    uint8_t data[3];
    int ret = 0;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, HMC5883L_COMPAT)) {
        if (fdt_node_check_compatible(blob, offset, HMC5883_COMPAT)) {
            return 0;
        }
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

    /* Attempt read starting at IDA register */
    data[0] = HMC5883_IDA;
    err = i2c_ops->write(i2c, addr, data, 1);
    if (err != 1) {
        ret = 0;
        goto out;
    }

    /* Read IDA, IDB, IDC */
    err = i2c_ops->read(i2c, addr, data, 3);
    if (err != 3) {
        ret = 0;
        goto out;
    }

    /* Ensure all three registers contain the expected values */
    ret = (data[0] == HMC5883_IDA_VAL) && (data[1] == HMC5883_IDB_VAL) &&
            (data[2] == HMC5883_IDC_VAL);

out:
    device_put(i2c_obj);
out_free_parent:
    free(parent);
    return ret;
}

static struct obj *hmc5883_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, err;
    uint8_t is_hmc5883l;
    char *parent;
    struct obj *mag_obj;
    struct mag *mag;
    struct hmc5883 *hmc_mag;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    /* Is this the HMC5883 or HMC5883L? */
    if (!fdt_node_check_compatible(blob, offset, HMC5883L_COMPAT)) {
        is_hmc5883l = 1;
    }
    else if (!fdt_node_check_compatible(blob, offset, HMC5883_COMPAT)) {
        is_hmc5883l = 0;
    }
    else {
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

    /* Instantiate an mag obj with hmc5883 ops */
    mag_obj = instantiate(name, &mag_class, &hmc5883_ops, struct mag);
    if (!mag_obj) {
        goto err_free_parent;
    }

    /* Connect mag to its parent I2C device */
    mag = to_mag(mag_obj);
    mag->device.parent = device_get(parent);
    if (!mag->device.parent) {
        goto err_free_obj;
    }

    /* Set up private data */
    mag->priv = kmalloc(sizeof(struct hmc5883));
    if (!mag->priv) {
        goto err_free_obj;
    }

    hmc_mag = (struct hmc5883 *) mag->priv;
    hmc_mag->ready = 0;
    hmc_mag->is_hmc5883l = is_hmc5883l;
    init_semaphore(&hmc_mag->lock);

    err = fdtparse_get_int(blob, offset, "reg", &hmc_mag->addr);
    if (err) {
        goto err_free_priv;
    }

    /* Export to the OS */
    class_export_member(mag_obj);

    free(parent);

    return mag_obj;

err_free_priv:
    kfree(mag->priv);
err_free_obj:
    kfree(get_container(mag_obj));
err_free_parent:
    free(parent);
    return NULL;
}

static struct semaphore hmc5883_driver_sem = INIT_SEMAPHORE;

static struct device_driver hmc5883_compat_driver = {
    .name = HMC5883_COMPAT,
    .probe = hmc5883_probe,
    .ctor = hmc5883_ctor,
    .class = &mag_class,
    .sem = &hmc5883_driver_sem,
};

static struct device_driver hmc5883l_compat_driver = {
    .name = HMC5883L_COMPAT,
    .probe = hmc5883_probe,
    .ctor = hmc5883_ctor,
    .class = &mag_class,
    .sem = &hmc5883_driver_sem,
};

/* Driver supports both hmc5883 and hmc5883l */
static int hmc5883_register(void) {
    device_compat_driver_register(&hmc5883_compat_driver);
    device_compat_driver_register(&hmc5883l_compat_driver);

    return 0;
}
CORE_INITIALIZER(hmc5883_register)
