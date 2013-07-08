#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>
#include <dev/sensors.h>
#include <dev/hw/i2c.h>
#include <arch/chip/dev/periph/px4_ms5611.h>

struct ms5611 {
    uint16_t c[7];
    int pressure;
    int temp;
    uint8_t sent;
};

#define MS5611_ADDR     0x76

int px4_ms5611_write(char c, void *env);
char px4_ms5611_read(void *env, int *error);
int px4_ms5611_close(resource *rd);

rd_t open_px4_ms5611(void) {
    rd_t ret;

    resource *new_r = create_new_resource();
    if (!new_r) {
        ret = -1;
        goto err;
    }

    struct ms5611 *env = (struct ms5611 *) kmalloc(sizeof(struct ms5611));
    if (!env) {
        ret = -1;
        goto err_free_new_r;
    }

    if (!(i2c2.ready)) {
        i2c2.init();
    }

    uint8_t packet;
    uint8_t data[2];

    acquire(&i2c2_semaphore);

    for (int i = 1; i <= 6; i++) {
        packet = 0xA0 + 2*i;  /* Read C[i] */
        i2c_write(&i2c2, MS5611_ADDR, &packet, 1);
        if (i2c_read(&i2c2, MS5611_ADDR, data, 2) != 2) {
            ret = -1;
            goto err_release_sem;
        }

        union {
            uint16_t val;
            uint8_t byte[2];
        } tmp = {0};

        tmp.byte[1] = data[0];
        tmp.byte[0] = data[1];

        env->c[i] = tmp.val;
    }

    release(&i2c2_semaphore);

    env->pressure = 0;
    env->sent = 0;

    new_r->env = (void *) env;
    new_r->writer = &px4_ms5611_write;
    new_r->reader = &px4_ms5611_read;
    new_r->closer = &px4_ms5611_close;
    new_r->read_sem = &i2c2_semaphore;
    new_r->write_sem = &i2c2_semaphore;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        ret = -1;
        goto err_free_env;
    }

    return ret;

err_release_sem:
    release(&i2c2_semaphore);
err_free_env:
    kfree(env);
err_free_new_r:
    kfree(new_r);
err:
    printk("OOPS: Unable to open MS5611.\r\n");
    return ret;
}

int px4_ms5611_write(char c, void *env) {
    /* Oh, no you don't */
    return -1;
}

char px4_ms5611_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    struct ms5611 *baro = (struct ms5611 *) env;

    if (!baro->sent) {
        uint8_t packet;
        uint8_t data[4] = {0};

        union {
            uint32_t val;
            uint8_t byte[4];
        } d1, d2 = {0};

        packet = 0x48;  /* Initiate pressure conversion */
        if (i2c_write(&i2c2, MS5611_ADDR, &packet, 1) < 0) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        /* Wait 10ms */
        /* This is a bit questionable, but since we don't need
         * I2C2 while we sleep, we release the semaphore before
         * the sleep. */
        release(&i2c2_semaphore);
        usleep(10000);
        acquire(&i2c2_semaphore);

        packet = 0x00;  /* Read ADC */
        if (i2c_write(&i2c2, MS5611_ADDR, &packet, 1) < 0) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        if (i2c_read(&i2c2, MS5611_ADDR, data, 3) != 3) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        d1.byte[3] = 0;
        d1.byte[2] = data[0];
        d1.byte[1] = data[1];
        d1.byte[0] = data[2];

        packet = 0x58;  /* Initiate temperature conversion */
        if (i2c_write(&i2c2, MS5611_ADDR, &packet, 1) < 0) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        /* Wait 10ms */
        release(&i2c2_semaphore);
        usleep(10000);
        acquire(&i2c2_semaphore);

        packet = 0x00;  /* Read ADC */
        if (i2c_write(&i2c2, MS5611_ADDR, &packet, 1) < 0) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        if (i2c_read(&i2c2, MS5611_ADDR, data, 3) != 3) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }

        d2.byte[3] = 0;
        d2.byte[2] = data[0];
        d2.byte[1] = data[1];
        d2.byte[0] = data[2];

        int64_t dT = d2.val - ((uint64_t) baro->c[5] << 8);
        int64_t temp = 2000 + ((dT * (uint64_t) baro->c[6]) >> 23);
        int64_t off = ((uint64_t) baro->c[2] << 16) + (((uint64_t) baro->c[4] * dT) >> 7);
        int64_t sens = ((uint64_t) baro->c[1] << 15) + (((uint64_t) baro->c[3] * dT) >> 8);

        /* Untested! It isn't cold enough in here */
        if (temp < 2000) {
            int64_t temp2;
            int64_t off2;
            int64_t sens2;

            temp2 = (dT * dT) >> 31;
            off2 = (5 * (temp - 2000) * (temp - 2000)) >> 1;
            sens2 = (5 * (temp - 2000) * (temp - 2000)) >> 2;

            if (temp < -1500) {
                off2 = off2 + 7 * (temp + 1500) * (temp + 1500);
                sens2 = sens2 + ((11 * (temp + 1500) * (temp + 1500)) >> 1);
            }

            temp = temp - temp2;
            off = off - off2;
            sens = sens - sens2;
        }

        int64_t pressure = ((d1.val * sens >> 21) - off) >> 15;

        baro->pressure = (int) pressure;
        baro->temp = (int) temp;
    }

    uint8_t index = baro->sent;

    baro->sent = (baro->sent + 1) % 8;

    if (index < 4) {
        int shift = 8*index;
        return (char) ((baro->pressure & (0xff << shift)) >> shift);
    }
    else {
        int shift = 8 * (index-4);
        return (char) ((baro->temp & (0xff << shift)) >> shift);
    }
}

int px4_ms5611_close(resource *rd) {
    free(rd->env);
    return 0;
}

int read_px4_ms5611(rd_t rd, struct barometer *baro) {
    if (!baro) {
        return -1;
    }

    union {
        int val[2];
        char byte[8];
    } data;

    if (read(rd, &data.byte[0], 8) != 8) {
        return -1;
    }

    int pressure = data.val[0];
    int temp = data.val[1];

    baro->pressure = pressure/100.0;
    baro->temp = temp/100.0;

    return 0;
}
