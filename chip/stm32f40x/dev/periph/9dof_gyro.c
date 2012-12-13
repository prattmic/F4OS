#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>

#include <dev/hw/i2c.h>
#include <dev/periph/9dof_gyro.h>

char sfe9dof_gyro_read(void *env) __attribute__((section(".kernel"))); 
void sfe9dof_gyro_write(char d, void *env) __attribute__((section(".kernel"))); 
void sfe9dof_gyro_close(resource *env) __attribute__((section(".kernel"))); 

rd_t open_sfe9dof_gyro(void) {
    sfe9dof_gyro *gyro = kmalloc(sizeof(sfe9dof_gyro)); 
    resource *new_r = create_new_resource();
    if(!new_r || !gyro) {
        panic_print("Could not allocate space for gyro resource.");
    }
    /* We expect that spi1 was init'd in bootmain.c */
    gyro->i2c_port = &i2c1;
    gyro->addr_ctr = 0;
    /* Address on I2C bus of gyro. */
    gyro->device_addr = 0x68;
    gyro->tmp_addr = 0x1D;

    if (!(gyro->i2c_port->ready)) {
        init_i2c1();
    }

    /* Fire it up, fire it up/ When we finally turn it over, make a b-line towards the boat, or... */
    uint8_t packet[2];
    packet[0] = 0x15;
    packet[1] = 0x07;
    i2c_write(gyro->i2c_port, gyro->device_addr, packet, 2);
    packet[0] = 0x16;
    packet[1] = 0x18;
    i2c_write(gyro->i2c_port, gyro->device_addr, packet, 2);

    new_r->env = gyro;
    new_r->writer = &sfe9dof_gyro_write;
    new_r->reader = &sfe9dof_gyro_read;
    new_r->closer = &sfe9dof_gyro_close;
    new_r->sem = &i2c1_semaphore;

    add_resource(curr_task->task, new_r);
    return curr_task->task->top_rd - 1;
}

char sfe9dof_gyro_read(void *env) {
    uint8_t tmp;
    sfe9dof_gyro *gyro = (sfe9dof_gyro *)env;
    if(gyro->addr_ctr > 5) {
        gyro->addr_ctr = 0;
    }
    gyro->tmp_addr = 0x1D;
    tmp = gyro->tmp_addr + gyro->addr_ctr++;
    i2c_write(gyro->i2c_port, gyro->device_addr, &tmp, 1);
    return (char) i2c_read(gyro->i2c_port, gyro->device_addr);
}

void sfe9dof_gyro_write(char d, void *env) {
    /* No real meaning to this yet */
}

void sfe9dof_gyro_close(resource *res) {
    kfree(res->env);
}
