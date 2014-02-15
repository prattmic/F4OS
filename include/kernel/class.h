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

/*
 * CLASS - a group of related objs and a way to operate on them - Michael Yenik
 *
 * "classes" provide a useful abstraction from objs. A class is meant to have
 * one obj_type and contain a collection of all objs that have that type
 * (all "instances" of that class, if you'll excuse the OO terminology).
 * Classes also provide a collection of operations that all objs of the
 * class's type should be able to do ("methods"). These are pointers to
 * implementation-specific versions of the function, which depend on that
 * obj's backing driver.
 *
 * Classes might be used for something like:
 * Accelerometers
 * ~ Give a set of methods like initialize, get_data, ...
 * ~ These methods change from obj to obj, each specific accelerometer instance
 *   may provide different pointers to a common per-class set of operations
 * Resources
 * ~ Give a set of methods all resources implement (read, write...)
 * ~ Each resource will wrap some device or underlying thing, and so
 *   they each have their own read/write
 *
 * CLASS API
 * struct obj *instantiate(char *name, struct class *class, void *ops, type)
 */

#ifndef KERNEL_CLASS_H_INCLUDED
#define KERNEL_CLASS_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>
#include <kernel/collection.h>

typedef struct class {
    struct collection instances;
    struct obj_type *type;
    struct obj      obj;
} class_t;

#define to_class(__obj) container_of((__obj), struct class, obj)

#define get_by_name_from_class(__name, __cls) \
    get_by_name((__name), &(__cls)->instances)

/*
 * Instantiate a class member
 *
 * Allocate (using kmalloc()) an instance of a class, using the container
 * type, returning the instance obj.
 *
 * When finished with the class instance, the container should be freed
 * with kfree().
 *
 * NOTE: The container type is not returned, the obj within the container.
 * When freeing, use get_container() to get the address to free.
 *
 * @param name  Instance name
 * @param class struct class of class type to instantiate
 * @param ops   Class ops struct
 * @param type  Type of class container
 * @returns pointer to instance obj, or NULL on error
 */
#define instantiate(name, class, ops, type) \
    __instantiate(name, class, ops, sizeof(type))

struct obj *__instantiate(const char *name, struct class *class, void *ops,
                          size_t size);

/*
 * Make an instantiated class member visible to the rest of the OS.
 *
 * Until this function is called, the class member is not visible
 * unless someone has an explicit reference to it.
 *
 * @param o Class member to export
 * @returns zero on success, negative on error
 */
int class_export_member(struct obj *o);

#define INIT_CLASS(symbol, name, obj_type) {    \
    .instances = INIT_COLLECTION((symbol).instances), \
    .type = (obj_type),   \
    .obj = INIT_OBJ((symbol).obj, (name), (obj_type), NULL, NULL),  \
}

#endif /* KERNEL_CLASS_H_INCLUDED */
