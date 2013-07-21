#include <list.h>
#include <string.h>
#include <kernel/class.h>
#include <kernel/obj.h>
#include <kernel/semaphore.h>
#include <dev/device.h>

struct list drivers = INIT_LIST(drivers);
struct semaphore driver_sem = INIT_SEMAPHORE;

/* Find and construct a device */
struct obj *device_get(const char *name) {
    struct device_driver *driver = NULL;
    struct obj *obj = NULL;
    int exists;

    acquire(&driver_sem);
    list_for_each_entry(driver, &drivers, list) {
        if (strncmp((char *)name, (char *)driver->name, 32) == 0) {
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
        return obj;
    }

    /* See if the device exists */
    exists = driver->probe(name);

    /* Construct a new instance of the device */
    if (exists) {
        obj = driver->ctor(name);
    }

    release(driver->sem);

    return obj;
}

void device_driver_register(struct device_driver *driver) {
    acquire(&driver_sem);
    list_add(&driver->list, &drivers);
    release(&driver_sem);
}
