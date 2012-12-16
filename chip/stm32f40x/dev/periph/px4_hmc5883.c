#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>
#include <dev/hw/i2c.h>
#include <dev/periph/px4_hmc5883.h>

#define HMC5883_ADDR    0x1E

struct hmc5883 {
    uint8_t reg;
};

static void px4_hmc5883_write(char c, void *env) __attribute__((section(".kernel")));
static char px4_hmc5883_read(void *env) __attribute__((section(".kernel")));
static void px4_hmc5883_close(resource *res) __attribute__((section(".kernel")));

rd_t open_px4_hmc5883(void) {
    resource *new_r = create_new_resource();
    struct hmc5883 *env = (struct hmc5883 *) malloc(sizeof(struct hmc5883));
    if (!new_r || !env) {
        panic_print("Could not allocate space for hmc5883 resource.");
    }

    if (!(i2c2.ready)) {
        i2c2.init();
    }

    uint8_t packet[4];

    packet[0] = 0x00;   /* Configuration register A */
    packet[1] = 0x78;   /* 8 samples/output, 75 Hz output, Normal measurement mode */
    packet[2] = 0x20;   /* auto-increment to config reg B; gain to 1090 */
    packet[3] = 0x00;   /* auto-increment to mode reg; continuous mode */
    i2c_write(&i2c2, HMC5883_ADDR, packet, 4);

    env->reg = 0x03;

    new_r->env = (void *) env;
    new_r->writer = &px4_hmc5883_write;
    new_r->reader = &px4_hmc5883_read;
    new_r->closer = &px4_hmc5883_close;
    new_r->sem = &i2c2_semaphore;

    add_resource(curr_task->task, new_r);
    return curr_task->task->top_rd - 1;
}

static void px4_hmc5883_write(char c, void *env) {
    /* Nope. */
}

static char px4_hmc5883_read(void *env) {
    struct hmc5883 *hmc = (struct hmc5883 *) env;

    i2c_write(&i2c2, HMC5883_ADDR, &hmc->reg, 1);

    char data = (char) i2c_read(&i2c2, HMC5883_ADDR);

    if (++hmc->reg > 0x08) {
        hmc->reg = 0x03;
    }

    return data;
}

static void px4_hmc5883_close(resource *res) {
    uint8_t packet[2];
    packet[0] = 0x02;   /* Mode register */
    packet[1] = 0x01;   /* Idle mode */
    i2c_write(&i2c2, HMC5883_ADDR, packet, 2);

    free(res->env);
}

/* Helper function - read all data and convert to gauss */
void read_px4_hmc5883(rd_t rd, struct magnetometer *mag) {
    char data[6];
    int16_t x,y,z;

    if (!mag) {
        return;
    }

    read(rd, data, 6);
    x = (data[0] << 8) | data[1];
    z = (data[2] << 8) | data[3];
    y = (data[4] << 8) | data[5];

    mag->x = ((float) x)/1090;
    mag->y = ((float) y)/1090;
    mag->z = ((float) z)/1090;
}
