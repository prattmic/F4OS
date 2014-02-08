/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

/**
 * Register a generic compatible string device driver with the OS
 *
 * Register a device driver for all compatible entries in the device tree.
 * The name field of the driver is the device tree compatible string that
 * the driver supports.  The fields in this driver will be used to create
 * standard device driver entries for each compatible device in the device
 * tree.
 *
 * Note that driver will be stored after return!  Do not pass one from
 * your stack!
 *
 * @param driver    Compatible string device driver to register
 */
void device_compat_driver_register(struct device_driver *driver);

/**
 * Automatically register device drivers from FDT
 *
 * Match nodes in the device tree to drivers registered as compatible
 * string drivers with device_compat_driver_register(), and register
 * those device drivers.
 */
void device_driver_fdt_register(void);

/**
 * Put an instance of a device
 *
 * When finished with a device, device_put returns it to the system,
 * possibly freeing its resources and destroying its object completely.
 *
 * The device must not be accessed after calling this function
 *
 * @param device    Device to put
 */
static inline void device_put(struct obj *device) {
    obj_put(device);
}

/**
 * List all device drivers for class
 *
 * Get a list of all registered device drivers that are for objects in a
 * given class.
 *
 * @param class Class to find devices for
 * @param names Output array of names, should be able to hold max names
 * @param max   Maximum number of names to place in names array
 *
 * @returns Total number of device drivers matching class, regardless of max
 */
int device_list_class(struct class *class, const char **names, int max);

#endif
