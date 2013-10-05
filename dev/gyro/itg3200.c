#include <board_config.h>
#include <dev/hw/i2c.h>
#include <dev/gyro.h>
#include <kernel/init.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#define ITG3200_ADDR    0x68    /* I2C address */
#define ITG3200_XOUT_H  0x1D    /* X output high data register */
#define ITG3200_GAIN    14.375  /* LSB/(deg/s) */

struct itg3200 {
    uint8_t             ready;
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
    ret = i2c_ops->write(i2c, ITG3200_ADDR, packet, 2);
    if (ret != 2) {
        goto err;
    }

    packet[0] = 0x15;   /* Sample Rate Divider Register */
    packet[1] = 0x07;   /* Sample rate = 1kHz / (7 + 1) = 125Hz */
    packet[2] = 0x18;   /* (Full Scale Register) Gyro full-scale range */
    ret = i2c_ops->write(i2c, ITG3200_ADDR, packet, 3);
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
    ret = i2c_ops->write(i2c, ITG3200_ADDR, packet, 2);

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
    ret = i2c_ops->write(i2c, ITG3200_ADDR, raw_data, 1);
    if (ret != 1) {
        goto out_err;
    }

    /* Read the six data registers */
    ret = i2c_ops->read(i2c, ITG3200_ADDR, raw_data, 6);
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

static int itg3200_probe(const char *name) {
    /* Check if the board has a valid config for the gyronetometer. */
    return itg3200_gyro_config.valid == BOARD_CONFIG_VALID_MAGIC;
}

static struct obj *itg3200_ctor(const char *name) {
    struct obj *gyro_obj;
    struct gyro *gyro;
    struct itg3200 *itg_gyro;

    /* Check if the board has a valid config for the gyronetometer. */
    if (itg3200_gyro_config.valid != BOARD_CONFIG_VALID_MAGIC) {
        return NULL;
    }

    /* Instantiate an gyro obj with itg3200 ops */
    gyro_obj = instantiate((char *)name, &gyro_class, &itg3200_ops, struct gyro);
    if (!gyro_obj) {
        return NULL;
    }

    /* Connect gyro to its parent I2C device */
    gyro = to_gyro(gyro_obj);
    gyro->device.parent = device_get(itg3200_gyro_config.parent_name);
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

    /* Export to the OS */
    class_export_member(gyro_obj);

    return gyro_obj;

err_free_obj:
    kfree(gyro_obj);
    return NULL;
}

static struct semaphore itg3200_driver_sem = INIT_SEMAPHORE;

static struct device_driver itg3200_driver = {
    .name = "itg3200",
    .probe = itg3200_probe,
    .ctor = itg3200_ctor,
    .class = &gyro_class,
    .sem = &itg3200_driver_sem,
};

static int itg3200_register(void) {
    device_driver_register(&itg3200_driver);

    return 0;
}
CORE_INITIALIZER(itg3200_register)
