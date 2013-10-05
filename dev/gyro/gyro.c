#include <dev/gyro.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void gyro_dtor(struct obj *o);

struct obj_type gyro_type_s  = {
    .offset = offset_of(struct gyro, obj),
    .dtor = gyro_dtor,
};

/*
 * Gyroscopes are never destroyed.  When their refcount reaches zero,
 * this destructor is called, and the gyroscope is deinitialized (possibly
 * going into a low power state), but they remain registered and available for
 * use for the duration of the OS.
 */
void gyro_dtor(struct obj *o) {
    struct gyro *gyro;
    struct gyro_ops *ops;

    assert_type(o, &gyro_type_s);
    gyro = to_gyro(o);
    ops = (struct gyro_ops *)o->ops;
    ops->deinit(gyro);
}

struct class gyro_class = INIT_CLASS(gyro_class, "gyro", &gyro_type_s);

int gyro_setup(void) {
    obj_init(&gyro_class.obj, system_class.type, "gyro");

    register_with_system(&dev_system, &gyro_class);
    return 0;
}
CORE_INITIALIZER(gyro_setup)
