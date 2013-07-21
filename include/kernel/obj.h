/*
 * OBJ IMPLEMENTATION - Michael Yenik
 *
 * An obj is a "thing", meant to be very abstract and useful when used
 * with other parts of the kernel. The main features it has are:
 *
 * ~ A reference count, so that its lifecycle is well defined and it need
 * not be meticulously tracked as it passes from queue to queue
 *
 * ~ A "type". Not totally in the programming language sense, all objs
 * reference an obj_type, which describes how this particular type of thing
 * that the obj is in should be destroyed when it is no longer referenced, what
 * specific operations (functions) should be used when interacting with this
 * particular obj's container, and how to unpack the container from an obj
 * reference.
 *
 *
 * THE OBJ API (should never change)
 *
 * void obj_release(struct obj *o)
 * ~~ decrement obj refcount and call destructor if possible
 *
 * void obj_init(struct obj *o)
 * ~~ used to properly initialize embedded obj in some other struct
 *
 * char *obj_get_name(struct obj *o)
 * ~~ returns name of obj
 *
 * void obj_set_name(struct obj *o, char *name)
 * ~~ sets name of obj
 *
 * void obj_get(struct obj *o)
 * ~~ increments refcount and claims as being used
 *
 * void *get_container(struct obj *o)
 * ~~ gets object container using type's offset
 *
 */

#ifndef KERNEL_OBJ_H_INCLUDED
#define KERNEL_OBJ_H_INCLUDED

#include <stdint.h>
#include <list.h>
#include <atomic.h>
#include <kernel/fault.h> /* kind of hacky, for panic_print in assert */

#define assert_type(obj, __type) if((obj)->type != (__type)) \
        panic_print("Type assertion failed for object %s!\n", (obj)->name)

struct obj_type;

typedef struct obj {
    atomic_t        refcount;
    char            *name;
    struct obj      *parent;    /* Parent in some sense, often a class */
    struct obj_type *type;
    struct list     list;
    void            *ops;       /* Operations that this obj supports, typed from class */
} obj_t;

typedef struct obj_type {
    void        (*dtor)(struct obj *);
    uintptr_t   offset;
} obj_type_t;

#define INIT_OBJ(symbol, obj_name, obj_type, obj_ops, obj_parent) { \
    .refcount = ATOMIC_INIT(1),  \
    .name = obj_name,   \
    .parent = obj_parent,   \
    .type = obj_type,   \
    .list = INIT_LIST(symbol.list), \
    .ops = obj_ops, \
}

/* Implementations for obj API */

static inline void obj_init(struct obj *o, struct obj_type *type, char *name) {
    atomic_set(&o->refcount, 1);
    o->type = type;
    o->name = name;
    o->list.next = &o->list;
    o->list.prev = &o->list;
}

static inline void obj_set_name(struct obj *o, char *name) {
    o->name = name;
}

static inline char *obj_get_name(struct obj *o) {
    return o->name;
}

static inline void obj_get(struct obj *o) {
    atomic_inc(&o->refcount);
}

static inline void obj_put(struct obj *o) {
    if(atomic_dec_and_test(&o->refcount)) {
        o->type->dtor(o);
    }
}

static inline void *get_container(struct obj *o) {
    uintptr_t offset = (uintptr_t)o->type->offset;
    return (void *)((uintptr_t)o - offset);
}

#endif
