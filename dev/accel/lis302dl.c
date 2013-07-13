#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
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

#include "lis302dl.h"

/*
 * STMicro LIS302DL Accelerometer Driver
 */

struct lis302dl {
    struct spi_dev spi_dev;
    int ready;
};

/* XXX: Currently hard-coded for STM32 Discovery */
static void lis302dl_cs_high(void) {
    *GPIO_ODR(GPIOE) |= GPIO_ODR_PIN(3);
}

static void lis302dl_cs_low(void) {
    *GPIO_ODR(GPIOE) &= ~(GPIO_ODR_PIN(3));
}

/* XXX: Currently hard-coded for STM32 Discovery */
static int lis302dl_init(struct accel *a) {
    struct spi *spi = (struct spi *) to_spi(a->device.parent);
    struct spi_ops *spi_ops = (struct spi_ops *)spi->obj.ops;
    struct lis302dl *lis_accel = (struct lis302dl *) a->priv;

    /* Set up CS pin and set high */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    /* PE3 */
    gpio_moder(GPIOE, 3, GPIO_MODER_OUT);
    gpio_otyper(GPIOE, 3, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOE, 3, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOE, 3, GPIO_OSPEEDR_50M);

    /* Properly idle CS */
    lis302dl_cs_high();

    lis_accel->spi_dev.cs_high = lis302dl_cs_high;
    lis_accel->spi_dev.cs_low = lis302dl_cs_low;
    lis_accel->spi_dev.extended_transaction = 0;

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
        struct spi *spi = (struct spi *) to_spi(a->device.parent);
        struct spi_ops *spi_ops = (struct spi_ops *)spi->obj.ops;

        /* Power down mode */
        uint8_t data[2] = {
            LIS302DL_CTRL1,
            0x00,
        };
        ret = spi_ops->write(spi, &lis_accel->spi_dev, data, 2);
    }

    return ret;
}

static int lis302dl_get_raw_data(struct accel *a, struct accel_raw_data *data) {
    struct spi *spi = (struct spi *) to_spi(a->device.parent);
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

/* XXX: Currently hard-coded for STM32 Discovery (spi1) */
int create_lis302dl(void) {
    /* Instantiate an accel with lis302dl ops */
    struct obj *o = instantiate("lis302dl", &accel_class, &lis302dl_ops, struct accel);
    if (!o) {
        return -1;
    }

    struct accel *a = container_of(o, struct accel, obj);

    a->device.parent = get_by_name("spi1", &spi_class.instances);

    a->priv = kmalloc(sizeof(struct lis302dl));
    if (!a->priv) {
        /* TODO: Uninstantiate */
        return -1;
    }

    struct lis302dl *lis_accel = (struct lis302dl *) a->priv;
    lis_accel->ready = 0;

    /* Export to the OS */
    class_export_member(o);

    return 0;
}
LATE_INITIALIZER(create_lis302dl)
