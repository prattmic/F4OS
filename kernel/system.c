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

#include <kernel/system.h>
#include <kernel/init.h>
#include <stdio.h>

struct collection systems = INIT_COLLECTION(systems);

struct obj_type system_type_s = {
    .dtor = NULL,
    .offset = offset_of(system_t, obj),
};

struct class system_class = INIT_CLASS(system_class, "system", &system_type_s);

struct system dev_system = INIT_SYSTEM(dev_system, dev);

int create_dev_system(void) {
    obj_init(&dev_system.obj, &system_type_s, "dev");
    collection_add(&systems, &dev_system.obj);
    return 0;
}
CORE_INITIALIZER(create_dev_system)

struct obj *get_by_name_from_system(struct system *sys, char *cls_name, char *inst_name) {
    struct obj *cls_obj = get_by_name(cls_name, &sys->classes);

    if(!cls_obj)
        return NULL;

    struct class *cls = to_class(cls_obj);
    return get_by_name_from_class(inst_name, cls);
}

struct obj *get_system_by_name(char *name) {
    return get_by_name(name, &systems);
}

void register_with_system(struct system *sys, struct class *cls) {
    collection_add(&sys->classes, &cls->obj);
}
