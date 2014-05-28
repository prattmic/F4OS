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
#include <dev/accel.h>
#include <dev/device.h>
#include <dev/fdtparse.h>
#include <dev/mpu6000/class.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>
#include "regs.h"

#define MPU6000_ACCEL_COMPAT    "invensense,mpu6000-accel"

struct mpu6000_accel {
    int ready;
};

static int mpu6000_accel_init(struct accel *accel) {
    struct mpu6000 *mpu = to_mpu6000(accel->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_accel *mpu_accel = (struct mpu6000_accel *) accel->priv;
    int ret;

    ret = mpu_ops->enable_accelerometer(mpu, 1);
    if (ret) {
        return ret;
    }

    ret = mpu_ops->write_reg(mpu, MPU6000_ACCEL_CONFIG,
                                  MPU6000_ACCEL_CONFIG_4G);
    if (ret) {
        mpu_ops->enable_accelerometer(mpu, 0);
        return ret;
    }

    mpu_accel->ready = 1;

    return 0;
}

static int mpu6000_accel_deinit(struct accel *accel) {
    struct mpu6000 *mpu = to_mpu6000(accel->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_accel *mpu_accel = (struct mpu6000_accel *) accel->priv;

    mpu_accel->ready = 0;

    return mpu_ops->enable_accelerometer(mpu, 0);
}

static int mpu6000_accel_get_raw_data(struct accel *accel,
                                      struct accel_raw_data *data) {
    struct mpu6000 *mpu = to_mpu6000(accel->device.parent);
    struct mpu6000_ops *mpu_ops = (struct mpu6000_ops *) mpu->obj.ops;
    struct mpu6000_accel *mpu_accel = (struct mpu6000_accel *) accel->priv;
    uint8_t raw_data[6];

    if (!mpu_accel->ready) {
        struct accel_ops *ops = (struct accel_ops *) accel->obj.ops;
        ops->init(accel);
    }

    if (mpu_ops->read_regs(mpu, MPU6000_ACCEL_XOUT_H, raw_data, 6) != 6) {
        return -1;
    }

    data->x = (int16_t) (raw_data[0] << 8) | raw_data[1];
    data->y = (int16_t) (raw_data[2] << 8) | raw_data[3];
    data->z = (int16_t) (raw_data[4] << 8) | raw_data[5];

    return 0;
}

static int mpu6000_accel_get_data(struct accel *accel,
                                  struct accel_data *data) {
    struct accel_ops *ops = (struct accel_ops *) accel->obj.ops;
    struct accel_raw_data raw_data;

    if (ops->get_raw_data(accel, &raw_data)) {
        return -1;
    }

    data->x = raw_data.x/MPU6000_ACCEL_SENSITIVITY_4G;
    data->y = raw_data.y/MPU6000_ACCEL_SENSITIVITY_4G;
    data->z = raw_data.z/MPU6000_ACCEL_SENSITIVITY_4G;

    return 0;
}

struct accel_ops mpu6000_accel_ops = {
    .init = mpu6000_accel_init,
    .deinit = mpu6000_accel_deinit,
    .get_data = mpu6000_accel_get_data,
    .get_raw_data = mpu6000_accel_get_raw_data,
};

/* Verify parent MPU6000 exists */
static int mpu6000_accel_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset, ret;
    char *parent;
    struct obj *parent_obj;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_ACCEL_COMPAT)) {
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
        /* No MPU6000, no MPU6000 accelerometer */
        ret = 0;
    }

    free(parent);

    return ret;
}

static struct obj *mpu6000_accel_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, parent_offset;
    char *parent;
    struct obj *accel_obj;
    struct accel *accel;
    struct mpu6000_accel *mpu_accel;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, MPU6000_ACCEL_COMPAT)) {
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

    accel_obj = instantiate(name, &accel_class, &mpu6000_accel_ops,
                            struct accel);
    if (!accel_obj) {
        goto err_free_parent;
    }

    /* Connect to parent MPU6000 */
    accel = to_accel(accel_obj);
    accel->device.parent = device_get(parent);
    if (!accel->device.parent) {
        goto err_free_obj;
    }

    /* Set up private data */
    accel->priv = kmalloc(sizeof(struct mpu6000_accel));
    if (!accel->priv) {
        goto err_free_obj;
    }

    mpu_accel = (struct mpu6000_accel *) accel->priv;
    mpu_accel->ready = 0;

    /* Export to the OS */
    class_export_member(accel_obj);

    free(parent);

    return accel_obj;

err_free_obj:
    class_deinstantiate(accel_obj);
err_free_parent:
    free(parent);
    return NULL;
}

static struct mutex mpu6000_accel_driver_mut = INIT_MUTEX;

static struct device_driver mpu6000_accel_compat_driver = {
    .name = MPU6000_ACCEL_COMPAT,
    .probe = mpu6000_accel_probe,
    .ctor = mpu6000_accel_ctor,
    .class = &accel_class,
    .mut = &mpu6000_accel_driver_mut,
};

static int mpu6000_accel_register(void) {
    device_compat_driver_register(&mpu6000_accel_compat_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_accel_register)
