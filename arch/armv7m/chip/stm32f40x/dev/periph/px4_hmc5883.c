#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <dev/hw/i2c.h>
#include <arch/chip/dev/periph/px4_hmc5883.h>

#define HMC5883_ADDR    0x1E

struct hmc5883 {
    uint8_t reg;
};

static int px4_hmc5883_write(char c, void *env) __attribute__((section(".kernel")));
static char px4_hmc5883_read(void *env, int *error) __attribute__((section(".kernel")));
static int px4_hmc5883_close(resource *res) __attribute__((section(".kernel")));

rd_t open_px4_hmc5883(void) {
    rd_t ret;

    resource *new_r = create_new_resource();
    if (!new_r) {
        ret = -1;
        goto err;
    }

    struct hmc5883 *env = (struct hmc5883 *) malloc(sizeof(struct hmc5883));
    if (!env) {
        ret = -1;
        goto err_free_new_r;
    }

    if (!(i2c2.ready)) {
        i2c2.init();
    }

    acquire(&i2c2_semaphore);

    uint8_t packet[4];
    packet[0] = 0x00;   /* Configuration register A */
    packet[1] = 0x78;   /* 8 samples/output, 75 Hz output, Normal measurement mode */
    packet[2] = 0x20;   /* auto-increment to config reg B; gain to 1090 */
    packet[3] = 0x00;   /* auto-increment to mode reg; continuous mode */
    if (i2c_write(&i2c2, HMC5883_ADDR, packet, 4) != 4) {
        ret = -1;
        goto err_release_sem;
    }

    release(&i2c2_semaphore);

    env->reg = 0x03;

    new_r->env = (void *) env;
    new_r->writer = &px4_hmc5883_write;
    new_r->reader = &px4_hmc5883_read;
    new_r->closer = &px4_hmc5883_close;
    new_r->read_sem = &i2c2_semaphore;
    new_r->write_sem = &i2c2_semaphore;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        goto err_free_env;
    }

    return ret;

err_release_sem:
    release(&i2c2_semaphore);
err_free_env:
    free(env);
err_free_new_r:
    kfree(new_r);
err:
    printk("OOPS: Unable to open HMC5883.\r\n");
    return ret;
}

static int px4_hmc5883_write(char c, void *env) {
    /* Nope. */
    return -1;
}

static char px4_hmc5883_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    struct hmc5883 *hmc = (struct hmc5883 *) env;

    if (i2c_write(&i2c2, HMC5883_ADDR, &hmc->reg, 1) != 1) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    uint8_t data;
    if (i2c_read(&i2c2, HMC5883_ADDR, &data, 1) != 1) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    if (++hmc->reg > 0x08) {
        hmc->reg = 0x03;
    }

    return data;
}

static int px4_hmc5883_close(resource *res) {
    uint8_t packet[2];
    packet[0] = 0x02;   /* Mode register */
    packet[1] = 0x01;   /* Idle mode */
    i2c_write(&i2c2, HMC5883_ADDR, packet, 2);

    free(res->env);
    return 0;
}

/* Helper function - read all data and convert to gauss */
int read_px4_hmc5883(rd_t rd, struct magnetometer *mag) {
    char data[6];
    int16_t x,y,z;

    if (!mag) {
        return -1;
    }

    if (read(rd, data, 6) != 6) {
        return -1;
    }

    x = (data[0] << 8) | data[1];
    z = (data[2] << 8) | data[3];
    y = (data[4] << 8) | data[5];

    mag->x = ((float) x)/1090;
    mag->y = ((float) y)/1090;
    mag->z = ((float) z)/1090;

    return 0;
}
