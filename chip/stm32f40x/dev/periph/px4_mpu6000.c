#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mm/mm.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/registers.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <dev/hw/gpio.h>
#include <dev/hw/spi.h>

#include <dev/periph/px4_mpu6000.h>

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
        printk("OOPS: Could not allocate space for mpu6000 resource.\r\n");
        return -1;
    }

    struct mpu6000 *env = (struct mpu6000 *) kmalloc(sizeof(struct mpu6000));
    if (!env) {
        printk("OOPS: Could not allocate space for mpu6000 resource.\r\n");
        kfree(new_r);
        return -1;
    }

    env->spi_port = &spi1;
    if (!env->spi_port->ready) {
        env->spi_port->init();
    }

    env->spi_dev.cs_high = &cs_high;
    env->spi_dev.cs_low = &cs_low;
    env->read_ctr = 0;

    acquire(&spi1_semaphore);

    /* Active mode, clock with gyro X reference */
    uint8_t data = MPU6000_PWR_MGMT_1_CLK_PLLGYROX;
    if (spi_write(env->spi_port, &env->spi_dev, MPU6000_PWR_MGMT_1, &data, 1) != 1) {
        /* Unable to activate :( */
        release(&spi1_semaphore);
        kfree(env);
        kfree(new_r);
        return -1;
    }

    release(&spi1_semaphore);

    /* Let clock settle */
    usleep(1000);

    acquire(&spi1_semaphore);

    /* 100Hz LPF, Gyro range +- 500deg/s, Accel range +-4g */
    uint8_t config[3] = {MPU6000_CONFIG_LPF_100HZ, MPU6000_GYRO_CONFIG_500DPS, MPU6000_ACCEL_CONFIG_4G};
    if (spi_write(env->spi_port, &env->spi_dev, MPU6000_CONFIG, config, 3) != 3) {
        data = MPU6000_PWR_MGMT_1_SLEEP;    /* Sleep mode */
        spi_write(env->spi_port, &env->spi_dev, MPU6000_PWR_MGMT_1, &data, 1);
        release(&spi1_semaphore);
        kfree(env);
        kfree(new_r);
        return -1;
    }

    release(&spi1_semaphore);

    new_r->env = (void *) env;
    new_r->reader = &px4_mpu6000_read;
    new_r->writer = &px4_mpu6000_write;
    new_r->closer = &px4_mpu6000_close;
    new_r->read_sem = &spi1_semaphore;
    new_r->write_sem = &spi1_semaphore;

    return add_resource(curr_task->task, new_r);
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
        if (spi_read(mpu->spi_port, &mpu->spi_dev, MPU6000_ACCEL_XOUT_H, mpu->data, 14) != 14) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }
    }

    char ret = (char) mpu->data[mpu->read_ctr++];

    if (mpu->read_ctr > 13) {
        mpu->read_ctr = 0;
    }

    return ret;
}

static int px4_mpu6000_write(char c, void *env) {
    /* Nope. */
    return -1;
}

static int px4_mpu6000_close(resource *res) {
    struct mpu6000 *mpu = (struct mpu6000 *) res->env;

    uint8_t data = MPU6000_PWR_MGMT_1_SLEEP;    /* Sleep mode */
    spi_write(mpu->spi_port, &mpu->spi_dev, MPU6000_PWR_MGMT_1, &data, 1);

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
