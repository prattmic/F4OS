#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <board_config.h>
#include <dev/hw/i2c.h>
#include <dev/mag.h>
#include <kernel/init.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#define HMC5883_ADDR    0x1E
#define HMC5883_GAIN    1090
#define HMC5883_OUTXA   0x03    /* First output register */

struct hmc5883 {
    uint8_t             ready;
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
    packet[1] = 0x78;   /* 8 samples/output, 75 Hz output, Normal measurement mode */
    packet[2] = 0x20;   /* auto-increment to config reg B; gain to 1090 */
    packet[3] = 0x00;   /* auto-increment to mode reg; continuous mode */
    ret = i2c_ops->write(i2c, HMC5883_ADDR, packet, 4);

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
    ret = i2c_ops->write(i2c, HMC5883_ADDR, packet, 2);

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
    ret = i2c_ops->write(i2c, HMC5883_ADDR, raw_data, 1);
    if (ret != 1) {
        goto out_err;
    }

    /* Read the six data registers */
    ret = i2c_ops->read(i2c, HMC5883_ADDR, raw_data, 6);
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
    struct mag_raw_data raw_data;
    int ret;

    ret = mag_ops->get_raw_data(mag, &raw_data);
    if (ret) {
        return ret;
    }

    data->x = ((float) raw_data.x)/HMC5883_GAIN;
    data->y = ((float) raw_data.y)/HMC5883_GAIN;
    data->z = ((float) raw_data.z)/HMC5883_GAIN;

    return 0;
}

struct mag_ops hmc5883_ops = {
    .init = hmc5883_init,
    .deinit = hmc5883_deinit,
    .get_data = hmc5883_get_data,
    .get_raw_data = hmc5883_get_raw_data,
};

static int hmc5883_probe(const char *name) {
    /* Check if the board has a valid config for the magnetometer. */
    return hmc5883_mag_config.valid == BOARD_CONFIG_VALID_MAGIC;
}

static struct obj *hmc5883_ctor(const char *name) {
    struct obj *mag_obj;
    struct mag *mag;
    struct hmc5883 *hmc_mag;

    /* Check if the board has a valid config for the magnetometer. */
    if (hmc5883_mag_config.valid != BOARD_CONFIG_VALID_MAGIC) {
        return NULL;
    }

    /* Instantiate an mag obj with hmc5883 ops */
    mag_obj = instantiate((char *)name, &mag_class, &hmc5883_ops, struct mag);
    if (!mag_obj) {
        return NULL;
    }

    /* Connect mag to its parent I2C device */
    mag = to_mag(mag_obj);
    mag->device.parent = device_get(hmc5883_mag_config.parent_name);
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
    init_semaphore(&hmc_mag->lock);

    /* Export to the OS */
    class_export_member(mag_obj);

    return mag_obj;

err_free_obj:
    kfree(mag_obj);
    return NULL;
}

static struct semaphore hmc5883_driver_sem = INIT_SEMAPHORE;

static struct device_driver hmc5883_driver = {
    .name = "hmc5883",
    .probe = hmc5883_probe,
    .ctor = hmc5883_ctor,
    .class = &mag_class,
    .sem = &hmc5883_driver_sem,
};

static int hmc5883_register(void) {
    device_driver_register(&hmc5883_driver);

    return 0;
}
CORE_INITIALIZER(hmc5883_register)
