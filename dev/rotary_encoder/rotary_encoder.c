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

#include <dev/rotary_encoder.h>
#include <kernel/init.h>
#include <kernel/class.h>
#include <kernel/system.h>
#include <mm/mm.h>

void rotary_encoder_dtor(struct obj *o);

struct obj_type rotary_encoder_type_s  = {
    .offset = offset_of(struct rotary_encoder, obj),
    .dtor = rotary_encoder_dtor,
};

/*
 * Rotary encoders are never destroyed.  When their refcount reaches zero,
 * this destructor is called, and the rotary encoder is deinitialized (possibly
 * going into a low power state), but it remains registered and available for
 * use for the duration of the OS.
 */
void rotary_encoder_dtor(struct obj *o) {
    struct rotary_encoder *rotary_encoder;
    struct rotary_encoder_ops *ops;

    assert_type(o, &rotary_encoder_type_s);
    rotary_encoder = to_rotary_encoder(o);
    ops = (struct rotary_encoder_ops *)o->ops;
    ops->deinit(rotary_encoder);
}

struct class rotary_encoder_class = INIT_CLASS(rotary_encoder_class,
                                               "rotary_encoder",
                                               &rotary_encoder_type_s);

/* Set up rotary encoder system */
int rotary_encoder_setup(void) {
    /* Set up rotary_encoder_class */
    obj_init(&rotary_encoder_class.obj, system_class.type, "rotary_encoder");

    register_with_system(&dev_system, &rotary_encoder_class);
    return 0;
}
CORE_INITIALIZER(rotary_encoder_setup)
