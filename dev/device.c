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

#include <libfdt.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/fdtparse.h>
#include <kernel/class.h>
#include <kernel/obj.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>
#include <dev/device.h>

struct list drivers = INIT_LIST(drivers);
struct semaphore driver_sem = INIT_SEMAPHORE;

struct list compat_drivers = INIT_LIST(compat_drivers);
struct semaphore compat_driver_sem = INIT_SEMAPHORE;

/* Find and construct a device */
struct obj *device_get(const char *name) {
    struct device_driver *iter, *driver = NULL;
    struct obj *obj = NULL;
    int exists;

    acquire(&driver_sem);
    list_for_each_entry(iter, &drivers, list) {
        if (strcmp(name, iter->name) == 0) {
            driver = iter;
            break;
        }
    }
    release(&driver_sem);

    /* No driver, too bad... */
    if (!driver) {
        return NULL;
    }

    /*
     * Use a driver-specific semaphore to prevent an instance of this
     * device from being constructed and added to the class after we
     * have decided that there is not an instance.  In such a case, the
     * device would get double constructed.
     */
    acquire(driver->sem);

    /* Try to find an existing instance */
    obj = get_by_name_from_class((char *)name, driver->class);
    /* Success! */
    if (obj) {
        obj_get(obj);
        goto out;
    }

    /* See if the device exists */
    exists = driver->probe(name);

    /* Construct a new instance of the device */
    if (exists) {
        obj = driver->ctor(name);
    }

out:
    release(driver->sem);

    return obj;
}

void device_driver_register(struct device_driver *driver) {
    acquire(&driver_sem);
    list_add(&driver->list, &drivers);
    release(&driver_sem);
}

void device_compat_driver_register(struct device_driver *driver) {
    acquire(&compat_driver_sem);
    list_add(&driver->list, &compat_drivers);
    release(&compat_driver_sem);
}

/**
 * Register a new device driver based on a device compatible driver
 *
 * Copy the passed driver, replacing name with the passed name, and register
 * the copied driver.
 *
 * @param driver    Comaptible string driver to copy
 * @param name      Name of new device driver.  A reference to this string will
 *                  be stored indefinitely.
 */
static void device_driver_register_from_compat(struct device_driver *driver,
                                               const char *name) {
    struct device_driver *new = kmalloc(sizeof(*new));
    if (!new) {
        fprintf(stderr, "%s: Unable to allocate device driver", __func__);
        return;
    }

    new->name = name;
    new->probe = driver->probe;
    new->ctor = driver->ctor;
    new->class = driver->class;
    new->sem = driver->sem;

    device_driver_register(new);
}

void device_driver_fdt_register(void) {
    const void *blob = fdtparse_get_blob();
    int offset = 0;

    /*
     * Nothing else should run while this does, but grab the semaphore
     * just to be safe.
     */
    acquire(&compat_driver_sem);

    /*
     * Walk every node in the device.  For those with a 'compatible'
     * property, check if any entries in the string list match any of
     * the registered compatible string device drivers.  If they do,
     * create and register a standard device driver for that node, based
     * on the compatible string driver.
     */
    do {
        const struct fdt_property *prop;
        const char *compat;
        int listlen, compatlen;

        offset = fdt_next_node(blob, offset, NULL);

        prop = fdt_get_property(blob, offset, "compatible", &listlen);
        if (!prop) {
            continue;
        }

        compat = (const char *)prop->data;
        while (listlen > 0) {
            struct device_driver *iter = NULL;
            list_for_each_entry(iter, &compat_drivers, list) {
                if (!strcmp(iter->name, compat)) {
                    /*
                     * TODO: Since we are walking the entire tree, we could
                     * just build the path as we go, rather than calling
                     * this expensive function, which will itself walk the
                     * entire tree.
                     */
                    char *name = fdtparse_get_path(blob, offset);
                    if (!name) {
                        fprintf(stderr, "%s: Unable to get name",
                                __func__);
                        goto next_node;
                    }

                    device_driver_register_from_compat(iter, name);
                    goto next_node;
                }
            }

            compatlen = strlen(compat);
            compat += compatlen + 1;
            listlen -= compatlen + 1;
        }

next_node:
        continue;
    } while (offset >= 0);

    release(&compat_driver_sem);
}

int device_list_class(struct class *class, const char **names, int max) {
    struct device_driver *driver;
    int total = 0;

    acquire(&driver_sem);
    list_for_each_entry(driver, &drivers, list) {
        if (driver->class == class) {
            if (total < max) {
                names[total] = driver->name;
            }
            total++;
        }
    }
    release(&driver_sem);

    return total;
}
