#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <mm/mm.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <dev/hw/spi.h>

#include <arch/chip/dev/periph/px4_mpu6000.h>

/* When set in register addres, perform a read instead of write */
#define SPI_READ    ((uint8_t) (1 << 7))

struct mpu6000 {
    struct spi_port *spi_port;
    struct spi_dev spi_dev;
    uint8_t read_ctr;
    uint8_t data[14];
};

static char px4_mpu6000_read(void *env, int *error);
static int px4_mpu6000_write(char c, void *env);
static int px4_mpu6000_close(resource *res);

static void cs_high(void) {
    *GPIO_ODR(GPIOB) |= GPIO_ODR_PIN(0);
}

static void cs_low(void) {
    *GPIO_ODR(GPIOB) &= ~(GPIO_ODR_PIN(0));
}

rd_t open_px4_mpu6000(void) {
    rd_t ret;

    /* Set up CS pin and set high */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB0 */
    gpio_moder(GPIOB, 0, GPIO_MODER_OUT);
    gpio_otyper(GPIOB, 0, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOB, 0, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, 0, GPIO_OSPEEDR_50M);

    /* idle CS */
    cs_high();

    resource *new_r = create_new_resource();
    if (!new_r) {
        ret = -1;
        goto err;
    }

    struct mpu6000 *env = (struct mpu6000 *) kmalloc(sizeof(struct mpu6000));
    if (!env) {
        ret = -1;
        goto err_free_new_r;
    }

    env->spi_port = &spi1;
    if (!env->spi_port->ready) {
        env->spi_port->init();
    }

    env->spi_dev.cs_high = &cs_high;
    env->spi_dev.cs_low = &cs_low;
    env->spi_dev.extended_transaction = 0;
    env->read_ctr = 0;

    acquire(&spi1_semaphore);

    /* Power management register 1 - Active mode, clock with gyro X reference */
    uint8_t power[2] = {MPU6000_PWR_MGMT_1, MPU6000_PWR_MGMT_1_CLK_PLLGYROX};
    if (spi_write(env->spi_port, &env->spi_dev, power, 2) != 2) {
        /* Unable to activate :( */
        ret = -1;
        goto err_release_sem;
    }

    release(&spi1_semaphore);

    /* Let clock settle */
    usleep(1000);

    acquire(&spi1_semaphore);

    /* Starting at config register;
     * 100Hz LPF,
     * Gyro range +- 500deg/s,
     * Accel range +-4g */
    uint8_t config[4] = {MPU6000_CONFIG,
        MPU6000_CONFIG_LPF_100HZ,
        MPU6000_GYRO_CONFIG_500DPS,
        MPU6000_ACCEL_CONFIG_4G};
    if (spi_write(env->spi_port, &env->spi_dev, config, 4) != 4) {
        ret = -1;
        goto err_sleep;
    }

    release(&spi1_semaphore);

    new_r->env = (void *) env;
    new_r->reader = &px4_mpu6000_read;
    new_r->writer = &px4_mpu6000_write;
    new_r->closer = &px4_mpu6000_close;
    new_r->read_sem = &spi1_semaphore;
    new_r->write_sem = &spi1_semaphore;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        acquire(&spi1_semaphore);
        goto err_sleep;
    }

    return ret;

err_sleep:
    power[1] = MPU6000_PWR_MGMT_1_SLEEP;    /* Sleep mode */
    spi_write(env->spi_port, &env->spi_dev, power, 2);
err_release_sem:
    release(&spi1_semaphore);
    kfree(env);
err_free_new_r:
    kfree(new_r);
err:
    printk("OOPS: Unable to open MPU6000.\r\n");
    return ret;
}

static char px4_mpu6000_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    if (!env) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    struct mpu6000 *mpu = (struct mpu6000 *) env;

    if (mpu->read_ctr == 0) {
        spi_start_transaction(mpu->spi_port, &mpu->spi_dev);
        /* Write start address */
        uint8_t addr = MPU6000_ACCEL_XOUT_H | SPI_READ;
        if (spi_write(mpu->spi_port, &mpu->spi_dev, &addr, 1) != 1) {
            goto spi_err;
        }
        /* Read 14 bytes */
        if (spi_read(mpu->spi_port, &mpu->spi_dev, mpu->data, 14) != 14) {
            goto spi_err;
        }
        spi_end_transaction(mpu->spi_port, &mpu->spi_dev);
    }

    char ret = (char) mpu->data[mpu->read_ctr++];

    if (mpu->read_ctr > 13) {
        mpu->read_ctr = 0;
    }

    return ret;

spi_err:
    spi_end_transaction(mpu->spi_port, &mpu->spi_dev);
    if (error != NULL) {
        *error = -1;
    }
    return 0;
}

static int px4_mpu6000_write(char c, void *env) {
    /* Nope. */
    return -1;
}

static int px4_mpu6000_close(resource *res) {
    struct mpu6000 *mpu = (struct mpu6000 *) res->env;

    /* Sleep mode */
    uint8_t power[2] = {MPU6000_PWR_MGMT_1, MPU6000_PWR_MGMT_1_SLEEP};
    spi_write(mpu->spi_port, &mpu->spi_dev, power, 2);

    kfree(res->env);
    return 0;
}

int read_px4_mpu6000(rd_t rd, struct accelerometer *accel, struct gyro *gyro, float *temp) {
    char data[14];

    if (read(rd, data, 14) != 14) {
        return -1;
    }

    int16_t accel_x = (data[0] << 8) | data[1];
    int16_t accel_y = (data[2] << 8) | data[3];
    int16_t accel_z = (data[4] << 8) | data[5];
    int16_t temp_out= (data[6] << 8) | data[7];
    int16_t gyro_x  = (data[8] << 8) | data[9];
    int16_t gyro_y  = (data[10] << 8) | data[11];
    int16_t gyro_z  = (data[12] << 8) | data[13];

    if (accel) {
        accel->x = accel_x/MPU6000_ACCEL_SENSITIVITY_4G;
        accel->y = accel_y/MPU6000_ACCEL_SENSITIVITY_4G;
        accel->z = accel_z/MPU6000_ACCEL_SENSITIVITY_4G;
    }

    if (gyro) {
        gyro->x = gyro_x/MPU6000_GYRO_SENSITIVITY_500DPS;
        gyro->y = gyro_y/MPU6000_GYRO_SENSITIVITY_500DPS;
        gyro->z = gyro_z/MPU6000_GYRO_SENSITIVITY_500DPS;
    }

    if (temp) {
        *temp = temp_out/340.0f + 36.53f;
    }

    return 0;
}
