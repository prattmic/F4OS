#include <stddef.h>
#include <string.h>
#include <dev/resource.h>
#include <arch/system.h>
#include <arch/chip/registers.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include "usbdev_internals.h"
#include "usbdev_desc.h"
#include <dev/hw/usbdev.h>

static int usbdev_resource_write(char c, void *env);
static int usbdev_resource_swrite(char *s, void *env);
static char usbdev_resource_read(void *env, int *error);
static int usbdev_resource_close(struct resource *resource);

struct semaphore usbdev_read_semaphore = INIT_SEMAPHORE;
struct semaphore usbdev_write_semaphore = INIT_SEMAPHORE;

resource usb_resource = {.writer     = &usbdev_resource_write,
                        .swriter    = &usbdev_resource_swrite,
                        .reader     = &usbdev_resource_read,
                        .closer     = &usbdev_resource_close,
                        .env        = NULL,
                        .read_sem   = &usbdev_read_semaphore,
                        .write_sem  = &usbdev_write_semaphore};

static int usbdev_resource_write(char c, void *env) {
    if (usb_ready) {
        return usbdev_write(&ep_tx, (uint8_t *) &c, 1);
    }
    else {
        return -1;
    }
}

static int usbdev_resource_swrite(char *s, void *env) {
    if (usb_ready) {
        return usbdev_write(&ep_tx, (uint8_t *) s, strlen(s));
    }
    else {
        return -1;
    }
}

static char usbdev_resource_read(void *env, int *error) {
    if (!usb_ready) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    if (error != NULL) {
        *error = 0;
    }

    while (ring_buf_empty(&ep_rx.rx)) {
        yield_if_possible();
    }

    char c = (char) ep_rx.rx.buf[ep_rx.rx.start];
    ep_rx.rx.start = (ep_rx.rx.start + 1) % ep_rx.rx.len;

    return c;
}

static int usbdev_resource_close(struct resource *resource) {
    printk("OOPS: USB is a fundamental resource, it may not be closed.");
    return -1;
}
