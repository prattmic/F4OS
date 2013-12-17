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

#include <stdint.h>
#include <compiler.h>
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
    g = to_gpio(o);
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
int __weak gpio_valid(uint32_t gpio) {
    return GPIO_ERR_INVAL;
}

struct obj __weak *_gpio_instantiate(uint32_t gpio) {
    return NULL;
}
