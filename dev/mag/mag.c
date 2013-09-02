#include <dev/mag.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void mag_dtor(struct obj *o);

struct obj_type mag_type_s  = {
    .offset = offset_of(struct mag, obj),
    .dtor = mag_dtor,
};

/*
 * Magnetometers are never destroyed.  When their refcount reaches zero,
 * this destructor is called, and the magnetometer is deinitialized (possibly
 * going into a low power state), but it remains registered and available for
 * use for the duration of the OS.
 */
void mag_dtor(struct obj *o) {
    struct mag *mag;
    struct mag_ops *ops;

    assert_type(o, &mag_type_s);
    mag = to_mag(o);
    ops = (struct mag_ops *)o->ops;
    ops->deinit(mag);
}

struct class mag_class = INIT_CLASS(mag_class, "mag", &mag_type_s);

/* Set up magnetometer system */
int mag_setup(void) {
    obj_init(&mag_class.obj, system_class.type, "mag");   /* Set up mag_class */

    /* In the future, system lookups are done by name and symbols are not exported */
    register_with_system(&dev_system, &mag_class);      /* Register mag_class with /system/dev */
    return 0;
}
CORE_INITIALIZER(mag_setup)
