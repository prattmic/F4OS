/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

#include <stdint.h>
#include <list.h>
#include <kernel/class.h>
#include <mm/mm.h>

struct obj *__instantiate(const char *name, struct class *class, void *ops,
                          size_t size) {
    struct obj *o;
    void *container;

    container = kmalloc(size);

    if(!container)
        return NULL;

    /* Hack, but needed for now */
    o = (struct obj *)((uintptr_t)container + class->type->offset);

    atomic_set(&o->refcount, 1);
    o->type = class->type;
    o->ops = ops;
    o->name = name;
    list_init(&o->list);

    /* add to class collection of instances */
    o->parent = &class->obj;

    return o;
}

int class_export_member(struct obj *o) {
    if (!o->parent) {
        return -1;
    }

    struct class *class = to_class(o->parent);
    collection_add(&class->instances, o);

    return 0;
}
