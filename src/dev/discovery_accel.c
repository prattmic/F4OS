#include "dev_header.h"
#include "spi.h"
#include "discovery_accel.h"

extern spi_dev spi1;

rd_t open_discovery_accel(void) {
    discovery_accel_setup();
    discovery_accel *accel = kmalloc(sizeof(discovery_accel));
    resource *new_r = kmalloc(sizeof(resource));
    /* We expect that spi1 was init'd in bootmain.c */
    accel->spi_port = &spi1;
    accel->read_ctr = 0;
    new_r->env = accel;
    new_r->writer = &discovery_accel_write;
    new_r->reader = &discovery_accel_read;
    new_r->sem = kmalloc(sizeof(semaphore));
    /* Just to be sure it's 0 */
    release(new_r->sem);
    add_resource(curr_task->task, new_r);
    return curr_task->task->top_rd - 1;
}

char discovery_accel_read(void *env) {
    discovery_accel *accel = (discovery_accel *)env;
    if(accel->read_ctr > 5)
        accel->read_ctr = 0;
    return (char)accel->spi_port->read(0x28 + accel->read_ctr++);
}

void discovery_accel_write(char d, void *env) {
    /* No real meaning to this yet */
}

