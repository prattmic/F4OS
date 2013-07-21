#ifndef INCLUDE_DEV_DEVICE_H_INCLUDED
#define INCLUDE_DEV_DEVICE_H_INCLUDED

#include <list.h>
#include <kernel/obj.h>
#include <kernel/semaphore.h>
#include <kernel/class.h>

/*
 * A device is a physical device connected to the system
 * through a physical parent.  All external communications
 * go through this parent.
 */

struct device {
    struct obj  *parent;
};

struct device_driver {
    const char          *name;
    int                 (*probe)(const char *);
    struct obj          *(*ctor)(const char *);
    struct class        *class;
    struct semaphore    *sem;
    struct list         list;
};

/**
 * Get an instance of a device
 *
 * Search through all available drivers to find a specific device.
 *
 * The device will be constructed if is it not already.
 *
 * @param name  Name of device to get
 * @return  Reference to object, or NULL on error
 */
struct obj *device_get(const char *name);

/**
 * Register a device driver with the OS
 *
 * Note that driver will be stored after return!  Do not pass one from
 * your stack!
 *
 * @param driver    Device driver to register
 */
void device_driver_register(struct device_driver *driver);

#endif
