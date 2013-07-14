#include <stdint.h>
#include <dev/hw/gpio.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <kernel/system.h>
#include <mm/mm.h>

void gpio_dtor(struct obj *o);

struct obj_type gpio_type_s  = {
    .offset = offset_of(struct gpio, obj),
    .dtor = gpio_dtor,
};

struct class gpio_class = INIT_CLASS(gpio_class, "gpio", &gpio_type_s);

void gpio_dtor(struct obj *o) {
    struct gpio *g;
    struct gpio_ops *ops;

    assert_type(o, &gpio_type_s);
    g = (struct gpio *) to_gpio(o);
    ops = (struct gpio_ops *)o->ops;
    ops->reset(g);
    ops->dtor(g);

    /* We are completely done with this object, get rid of it */
    collection_del(&gpio_class.instances, o);
    kfree(g);
}

int gpio_setup(void) {
    obj_init(&gpio_class.obj, system_class.type, "gpio");

    /* In the future, system lookups are done by name and symbols are not exported */
    register_with_system(&dev_system, &gpio_class);
    return 0;
}
CORE_INITIALIZER(gpio_setup)

/* A GPIO is only available if there is no obj instantiated for it */
int gpio_available(uint32_t gpio) {
    int ret = gpio_valid(gpio);
    if (ret) {
        return ret;
    }

    struct obj *curr = collection_iter(&gpio_class.instances);
    while (curr) {
        struct gpio *g = to_gpio(curr);

        if (g->num == gpio) {
            collection_stop(&gpio_class.instances);
            return GPIO_ERR_UNAVAIL;
        }

        curr = collection_next(&gpio_class.instances);
    }

    return 0;
}

struct obj *gpio_get(uint32_t gpio) {
    int ret = gpio_available(gpio);
    if (ret) {
        /* Error, GPIO not available */
        return NULL;
    }

    struct obj *o = _gpio_instantiate(gpio);
    if (!o) {
        return NULL;
    }

    struct gpio *g = to_gpio(o);

    g->num = gpio;
    g->active_low = 0;

    return o;
}

/* In the event that there is no chip GPIO driver, provide stub gpio_valid
 * and _gpio_instantiate that always return failure. */
int __attribute__((weak)) gpio_valid(uint32_t gpio) {
    return GPIO_ERR_INVAL;
}

struct obj __attribute__((weak)) *_gpio_instantiate(uint32_t gpio) {
    return NULL;
}
