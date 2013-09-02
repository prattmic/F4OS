#include <dev/accel.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void accel_dtor(struct obj *o);

struct obj_type accel_type_s  = {
    .offset = offset_of(struct accel, obj),
    .dtor = accel_dtor,
};

/*
 * Accelerometers are never destroyed.  When their refcount reaches zero,
 * this destructor is called, and the accelerometer is deinitialized (possibly
 * going into a low power state), but it remains registered and available for
 * use for the duration of the OS.
 */
void accel_dtor(struct obj *o) {
    struct accel *a;
    struct accel_ops *ops;

    assert_type(o, &accel_type_s);
    a = to_accel(o);
    ops = (struct accel_ops *)o->ops;
    ops->deinit(a);
}

struct class accel_class = INIT_CLASS(accel_class, "accel", &accel_type_s);

/* Set up accelerometer system */
int accel_setup(void) {
    obj_init(&accel_class.obj, system_class.type, "accel");   /* Set up accel_class */

    /* In the future, system lookups are done by name and symbols are not exported */
    register_with_system(&dev_system, &accel_class);      /* Register accel_class with /system/dev */
    return 0;
}
CORE_INITIALIZER(accel_setup)
