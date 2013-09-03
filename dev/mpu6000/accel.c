#include <stdint.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <dev/mpu6000/class.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>
#include "regs.h"

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

static int mpu6000_accel_probe(const char *name) {
    /* TODO: Check that parent exists */
    return 1;
}

static struct obj *mpu6000_accel_ctor(const char *name) {
    struct obj *accel_obj;
    struct accel *accel;
    struct mpu6000_accel *mpu_accel;

    accel_obj = instantiate((char *)name, &accel_class, &mpu6000_accel_ops,
                              struct accel);
    if (!accel_obj) {
        return NULL;
    }

    /*
     * Connect to parent MPU6000
     * TODO: Support more than SPI
     */
    accel = to_accel(accel_obj);
    accel->device.parent = device_get("mpu6000_spi");
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

    return accel_obj;

err_free_obj:
    kfree(accel_obj);
    return NULL;
}

static struct semaphore mpu6000_accel_driver_sem = INIT_SEMAPHORE;

static struct device_driver mpu6000_accel_driver = {
    .name = "mpu6000_accel",
    .probe = mpu6000_accel_probe,
    .ctor = mpu6000_accel_ctor,
    .class = &accel_class,
    .sem = &mpu6000_accel_driver_sem,
};

static int mpu6000_accel_register(void) {
    device_driver_register(&mpu6000_accel_driver);

    return 0;
}
CORE_INITIALIZER(mpu6000_accel_register)
