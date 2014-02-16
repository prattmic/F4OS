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
#include <dev/gyro.h>
#include <dev/mpu6000/class.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>
#include "regs.h"

#define MPU6000_GYRO_COMPAT    "invensense,mpu6000-gyro"

struct mpu6000_gyro {
    int ready;
};

static int mpu6000_gyro_init(struct gyro *gyro) {
    struct mpu6000 *mpu = to_mpu6000(gyro->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_gyro *mpu_gyro = (struct mpu6000_gyro *) gyro->priv;
    int ret;

    ret = mpu_ops->enable_gyroscope(mpu, 1);
    if (ret) {
        return ret;
    }

    ret = mpu_ops->write_reg(mpu, MPU6000_GYRO_CONFIG,
                                  MPU6000_GYRO_CONFIG_500DPS);
    if (ret) {
        mpu_ops->enable_gyroscope(mpu, 0);
        return ret;
    }

    mpu_gyro->ready = 1;

    return 0;
}

static int mpu6000_gyro_deinit(struct gyro *gyro) {
    struct mpu6000 *mpu = to_mpu6000(gyro->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_gyro *mpu_gyro = (struct mpu6000_gyro *) gyro->priv;

    mpu_gyro->ready = 0;

    return mpu_ops->enable_gyroscope(mpu, 0);
}

static int mpu6000_gyro_get_raw_data(struct gyro *gyro,
                                      struct gyro_raw_data *data) {
    struct mpu6000 *mpu = to_mpu6000(gyro->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_gyro *mpu_gyro = (struct mpu6000_gyro *) gyro->priv;
    uint8_t raw_data[6];

    if (!mpu_gyro->ready) {
        struct gyro_ops *ops = (struct gyro_ops *) gyro->obj.ops;
        ops->init(gyro);
    }

    if (mpu_ops->read_regs(mpu, MPU6000_GYRO_XOUT_H, raw_data, 6) != 6) {
        return -1;
    }

    data->x = (int16_t) (raw_data[0] << 8) | raw_data[1];
    data->y = (int16_t) (raw_data[2] << 8) | raw_data[3];
    data->z = (int16_t) (raw_data[4] << 8) | raw_data[5];

    return 0;
}

static int mpu6000_gyro_get_data(struct gyro *gyro,
                                  struct gyro_data *data) {
    struct gyro_ops *ops = (struct gyro_ops *) gyro->obj.ops;
    struct gyro_raw_data raw_data;

    if (ops->get_raw_data(gyro, &raw_data)) {
        return -1;
    }

    data->x = raw_data.x/MPU6000_GYRO_SENSITIVITY_500DPS;
    data->y = raw_data.y/MPU6000_GYRO_SENSITIVITY_500DPS;
    data->z = raw_data.z/MPU6000_GYRO_SENSITIVITY_500DPS;

    return 0;
}

struct gyro_ops mpu6000_gyro_ops = {
    .init = mpu6000_gyro_init,
    .deinit = mpu6000_gyro_deinit,
    .get_data = mpu6000_gyro_get_data,
    .get_raw_data = mpu6000_gyro_get_raw_data,
};

/* Verify parent MPU6000 exists */
static int mpu6000_gyro_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, ret;
    char *parent;
    struct obj *parent_obj;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_GYRO_COMPAT)) {
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

    /* Attempt to get parent MPU6000 */
    parent_obj = device_get(parent);
    if (parent_obj) {
        /* Found it!  Good to go! */
        device_put(parent_obj);
        ret = 1;
    }
    else {
        /* No MPU6000, no MPU6000 gyro */
        ret = 0;
    }

    free(parent);

    return ret;
}

static struct obj *mpu6000_gyro_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset;
    char *parent;
    struct obj *gyro_obj;
    struct gyro *gyro;
    struct mpu6000_gyro *mpu_gyro;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_GYRO_COMPAT)) {
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

    gyro_obj = instantiate(name, &gyro_class, &mpu6000_gyro_ops,
                           struct gyro);
    if (!gyro_obj) {
        goto err_free_parent;
    }

    /* Connect to parent MPU6000 */
    gyro = to_gyro(gyro_obj);
    gyro->device.parent = device_get(parent);
    if (!gyro->device.parent) {
        goto err_free_obj;
    }

    /* Set up private data */
    gyro->priv = kmalloc(sizeof(struct mpu6000_gyro));
    if (!gyro->priv) {
        goto err_free_obj;
    }

    mpu_gyro = (struct mpu6000_gyro *) gyro->priv;
    mpu_gyro->ready = 0;

    /* Export to the OS */
    class_export_member(gyro_obj);

    free(parent);

    return gyro_obj;

err_free_obj:
    class_deinstantiate(gyro_obj);
err_free_parent:
    free(parent);
    return NULL;
}

static struct semaphore mpu6000_gyro_driver_sem = INIT_SEMAPHORE;

static struct device_driver mpu6000_gyro_compat_driver = {
    .name = MPU6000_GYRO_COMPAT,
    .probe = mpu6000_gyro_probe,
    .ctor = mpu6000_gyro_ctor,
    .class = &gyro_class,
    .sem = &mpu6000_gyro_driver_sem,
};

static int mpu6000_gyro_register(void) {
    device_compat_driver_register(&mpu6000_gyro_compat_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_gyro_register)
