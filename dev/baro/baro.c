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
