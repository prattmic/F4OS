#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>

#include <dev/hw/spi.h>
#include <dev/periph/discovery_accel.h>

typedef struct discovery_accel {
        spi_dev *spi_port;
        uint8_t read_ctr;
} discovery_accel;

extern spi_dev spi1;

char discovery_accel_read(void *env) __attribute__((section(".kernel")));
void discovery_accel_write(char d, void *env) __attribute__((section(".kernel")));
void discovery_accel_close(resource *env) __attribute__((section(".kernel")));

rd_t open_discovery_accel(void) {
    discovery_accel *accel = kmalloc(sizeof(discovery_accel));
    resource *new_r = create_new_resource();
    if(!new_r || !accel) {
        panic_print("Could not allocate space for discovery accel resource");
    }
    /* We expect that spi1 was init'd in bootmain.c */
    accel->spi_port = &spi1;
    accel->spi_port->write(0x20, 0x47);
    accel->read_ctr = 0;

    new_r->env = accel;
    new_r->writer = &discovery_accel_write;
    new_r->reader = &discovery_accel_read;
    new_r->closer = &discovery_accel_close;
    new_r->sem = &spi1_semaphore;

    add_resource(curr_task->task, new_r);
    return curr_task->task->top_rd - 1;
}

char discovery_accel_read(void *env) {
    discovery_accel *accel = (discovery_accel *)env;
    if(accel->read_ctr > 2)
        accel->read_ctr = 0;
    return (char)accel->spi_port->read(0x29 + 2*accel->read_ctr++);
}

void discovery_accel_write(char d, void *env) {
    /* No real meaning to this yet */
}

void discovery_accel_close(resource *res) {
    kfree(res->env);
}
