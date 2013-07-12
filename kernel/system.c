#include <kernel/system.h>
#include <kernel/init.h>
#include <stdio.h>

struct collection systems = INIT_COLLECTION(systems);

struct obj_type system_type_s = {
    .dtor = NULL,
    .offset = offset_of(system_t, obj),
};

/* What to do with destroyed system? They could be made dynamically */
struct class system_class = {
    .dtor = NULL,
    .instances = INIT_COLLECTION(system_class.instances),
    .type = &system_type_s,
};

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
