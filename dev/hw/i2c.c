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

#include <dev/hw/i2c.h>
#include <kernel/class.h>
#include <mm/mm.h>

static void i2c_dtor(struct obj *o);

struct obj_type i2c_type_s  = {
    .offset = offset_of(struct i2c, obj),
    .dtor = &i2c_dtor,
};

struct class i2c_class = INIT_CLASS(i2c_class, "i2c", &i2c_type_s);

static void i2c_dtor(struct obj *o) {
    struct i2c *i2c;
    struct i2c_ops *ops;

    assert_type(o, &i2c_type_s);
    i2c = to_i2c(o);
    ops = (struct i2c_ops *) o->ops;
    ops->deinit(i2c);

    if (i2c->priv) {
        kfree(i2c->priv);
    }
}
