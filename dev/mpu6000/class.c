/*
 * Copyright (C) 2013 F4OS Authors
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
