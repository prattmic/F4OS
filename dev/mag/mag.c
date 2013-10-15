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
