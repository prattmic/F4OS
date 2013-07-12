#include <stdint.h>
#include <list.h>
#include <kernel/class.h>
#include <mm/mm.h>

struct obj *__instantiate(char *name, struct class *class, void *ops, size_t size) {
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
