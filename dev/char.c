/*
 * Copyright (C) 2014 F4OS Authors
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

#include <stdlib.h>
#include <dev/char.h>
#include <kernel/obj.h>

static void char_dtor(struct obj *o);

static struct obj_type char_type_s  = {
    .offset = offset_of(struct char_device, obj),
    .dtor = char_dtor,
};

/*
 * Cleanup internal structures and put the base obj before destroying it, but
 * do not explicitly destroy or deinitialize it, as there may be other users.
 */
static void char_dtor(struct obj *o) {
    struct char_device *c;
    struct char_ops *ops;

    assert_type(o, &char_type_s);
    c = to_char_device(o);
    ops = (struct char_ops *)o->ops;

    ops->_cleanup(c);

    if (c->base) {
        obj_put(c->base);
    }

    free(c);
}

struct char_device *char_device_create(struct obj *base,
                                       struct char_ops *ops) {
    struct char_device *c = malloc(sizeof(*c));
    if (!c) {
        return NULL;
    }

    obj_init(&c->obj, &char_type_s, "Character device");
    c->obj.ops = ops;
    c->base = base;

    return c;
}
