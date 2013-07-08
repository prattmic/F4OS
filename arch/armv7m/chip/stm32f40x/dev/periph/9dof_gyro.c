#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>

#include <dev/hw/i2c.h>
#include <arch/chip/dev/periph/9dof_gyro.h>

char sfe9dof_gyro_read(void *env, int *error) __attribute__((section(".kernel")));
int sfe9dof_gyro_write(char d, void *env) __attribute__((section(".kernel")));
int sfe9dof_gyro_close(resource *env) __attribute__((section(".kernel")));

rd_t open_sfe9dof_gyro(void) {
    rd_t ret;

    sfe9dof_gyro *gyro = kmalloc(sizeof(sfe9dof_gyro));
    if (!gyro) {
        ret = -1;
        goto err;
    }

    resource *new_r = create_new_resource();
    if(!new_r) {
        ret = -1;
        goto err_free_gyro;
    }

    /* We expect that spi1 was init'd in bootmain.c */
    gyro->i2c_port = &i2c1;
    gyro->addr_ctr = 0;
    /* Address on I2C bus of gyro. */
    gyro->device_addr = 0x68;
    gyro->tmp_addr = 0x1D;

    if (!(gyro->i2c_port->ready)) {
        gyro->i2c_port->init();
    }

    acquire(&i2c1_semaphore);

    /* Fire it up, fire it up/ When we finally turn it over, make a b-line towards the boat, or... */
    uint8_t packet[2];
    packet[0] = 0x15;
    packet[1] = 0x07;
    if (i2c_write(gyro->i2c_port, gyro->device_addr, packet, 2) != 2) {
        ret = -1;
        goto err_release_sem;
    }

    packet[0] = 0x16;
    packet[1] = 0x18;
    if (i2c_write(gyro->i2c_port, gyro->device_addr, packet, 2) != 2) {
        ret = -1;
        goto err_release_sem;
    }

    release(&i2c1_semaphore);

    new_r->env = gyro;
    new_r->writer = &sfe9dof_gyro_write;
    new_r->reader = &sfe9dof_gyro_read;
    new_r->closer = &sfe9dof_gyro_close;
    new_r->read_sem = &i2c1_semaphore;
    new_r->write_sem = &i2c1_semaphore;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        goto err_free_new_r;
    }

    return ret;

err_release_sem:
    release(&i2c1_semaphore);
err_free_new_r:
    kfree(new_r);
err_free_gyro:
    kfree(gyro);
err:
    printk("OOPS: Unable to open 9DOF gyro,\r\n");
    return ret;
}

char sfe9dof_gyro_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    uint8_t tmp;
    sfe9dof_gyro *gyro = (sfe9dof_gyro *)env;
    if(gyro->addr_ctr > 5) {
        gyro->addr_ctr = 0;
    }
    gyro->tmp_addr = 0x1D;
    tmp = gyro->tmp_addr + gyro->addr_ctr++;

    if (i2c_write(gyro->i2c_port, gyro->device_addr, &tmp, 1) != 1) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    uint8_t c;
    if (i2c_read(gyro->i2c_port, gyro->device_addr, &c, 1) != 1) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    return (char) c;
}

int sfe9dof_gyro_write(char d, void *env) {
    /* No real meaning to this yet */
    return -1;
}

int sfe9dof_gyro_close(resource *res) {
    kfree(res->env);
    return 0;
}
