#include <dev/baro.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void baro_dtor(struct obj *o);

struct obj_type baro_type_s  = {
    .offset = offset_of(struct baro, obj),
    .dtor = baro_dtor,
};

/*
 * Barometers are never destroyed.  When their refcount reaches zero,
 * this destructor is called, and the barometer is deinitialized (possibly
 * going into a low power state), but it remains registered and available for
 * use for the duration of the OS.
 */
void baro_dtor(struct obj *o) {
    struct baro *baro;
    struct baro_ops *ops;

    assert_type(o, &baro_type_s);
    baro = to_baro(o);
    ops = (struct baro_ops *) o->ops;
    ops->deinit(baro);
}

struct class baro_class = INIT_CLASS(baro_class, "baro", &baro_type_s);

/* Set up barometer system */
int baro_setup(void) {
    /* Set up baro_class */
    obj_init(&baro_class.obj, system_class.type, "baro");

    /* Register baro_class with /system/dev */
    register_with_system(&dev_system, &baro_class);
    return 0;
}
CORE_INITIALIZER(baro_setup)
