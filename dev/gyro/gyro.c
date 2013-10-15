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
