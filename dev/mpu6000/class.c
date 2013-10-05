#include <dev/mpu6000/class.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void mpu6000_dtor(struct obj *o);

struct obj_type mpu6000_type_s  = {
    .offset = offset_of(struct mpu6000, obj),
    .dtor = mpu6000_dtor,
};

/* MPU6000 objects are not destroyed, simply placed into a low power state */
void mpu6000_dtor(struct obj *o) {
    struct mpu6000 *m;
    struct mpu6000_ops *ops;

    assert_type(o, &mpu6000_type_s);
    m = to_mpu6000(o);
    ops = (struct mpu6000_ops *)o->ops;
    ops->deinit(m);
}

struct class mpu6000_class = INIT_CLASS(mpu6000_class, "mpu6000",
                                        &mpu6000_type_s);

int mpu6000_class_setup(void) {
    obj_init(&mpu6000_class.obj, system_class.type, "mpu6000");

    register_with_system(&dev_system, &mpu6000_class);
    return 0;
}
CORE_INITIALIZER(mpu6000_class_setup)
