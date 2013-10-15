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
