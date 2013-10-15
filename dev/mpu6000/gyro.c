/*
 * Copyright (C) 2013 F4OS Authors
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
#include <dev/device.h>
#include <dev/gyro.h>
#include <dev/mpu6000/class.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>
#include "regs.h"

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

static int mpu6000_gyro_probe(const char *name) {
    /* TODO: Check that parent exists */
    return 1;
}

static struct obj *mpu6000_gyro_ctor(const char *name) {
    struct obj *gyro_obj;
    struct gyro *gyro;
    struct mpu6000_gyro *mpu_gyro;

    gyro_obj = instantiate((char *)name, &gyro_class, &mpu6000_gyro_ops,
                              struct gyro);
    if (!gyro_obj) {
        return NULL;
    }

    /*
     * Connect to parent MPU6000
     * TODO: Support more than SPI
     */
    gyro = to_gyro(gyro_obj);
    gyro->device.parent = device_get("mpu6000_spi");
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

    return gyro_obj;

err_free_obj:
    kfree(gyro_obj);
    return NULL;
}

static struct semaphore mpu6000_gyro_driver_sem = INIT_SEMAPHORE;

static struct device_driver mpu6000_gyro_driver = {
    .name = "mpu6000_gyro",
    .probe = mpu6000_gyro_probe,
    .ctor = mpu6000_gyro_ctor,
    .class = &gyro_class,
    .sem = &mpu6000_gyro_driver_sem,
};

static int mpu6000_gyro_register(void) {
    device_driver_register(&mpu6000_gyro_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_gyro_register)
