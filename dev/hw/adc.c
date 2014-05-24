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

#include <libfdt.h>
#include <stdint.h>
#include <compiler.h>
#include <linker_array.h>
#include <dev/fdtparse.h>
#include <dev/hw/adc.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <kernel/system.h>
#include <mm/mm.h>

LINKER_ARRAY_DECLARE(adc_drivers)

void adc_dtor(struct obj *o);

struct obj_type adc_type_s  = {
    .offset = offset_of(struct adc, obj),
    .dtor = adc_dtor,
};

struct class adc_class = INIT_CLASS(adc_class, "adc", &adc_type_s);

void adc_dtor(struct obj *o) {
    struct adc *adc;
    struct adc_ops *ops;

    assert_type(o, &adc_type_s);
    adc = to_adc(o);
    ops = (struct adc_ops *)o->ops;
    ops->dtor(adc);

    /* Decref GPIO obj */
    obj_put(adc->gpio);

    /* We are completely done with this object, get rid of it */
    class_unexport_member(o);
    class_deinstantiate(o);
}

int adc_setup(void) {
    obj_init(&adc_class.obj, system_class.type, "adc");
    return 0;
}
CORE_INITIALIZER(adc_setup)

/* Lookup a device and driver to perform the ADC get with */
static struct obj *adc_get_lookup(const void *fdt, struct obj *gpio) {
    struct adc_driver *driver;
    int offset = 0;

    /*
     * Walk every node in the device, looking for a device compatible
     * with one of the drivers.
     *
     * When one is found, attempt to get an ADC obj from it.  If that
     * doesn't work, keep looking.
     */
    do {
        offset = fdt_next_node(fdt, offset, NULL);

        LINKER_ARRAY_FOR_EACH(adc_drivers, driver) {
            if (!fdt_node_check_compatible(fdt, offset, driver->compat)) {
                struct obj *obj = driver->adc_get(fdt, offset, gpio);
                if (obj) {
                    return obj;
                }
            }
        }
    } while (offset >= 0);

    return NULL;
}

struct obj *adc_get(struct obj *gpio, const char *driver_name) {
    const void *fdt = fdtparse_get_blob();
    struct adc_driver *driver = NULL;
    int offset;
    struct obj *adc;

    /* We will be holding onto the GPIO */
    obj_get(gpio);

    if (!driver_name) {
        adc = adc_get_lookup(fdt, gpio);
        if (!adc) {
            goto err;
        }
        return adc;
    }

    offset = fdt_path_offset(fdt, driver_name);
    if (offset < 0) {
        goto err;
    }

    LINKER_ARRAY_FOR_EACH(adc_drivers, driver) {
        if (!fdt_node_check_compatible(fdt, offset, driver->compat)) {
            break;
        }
    }

    if (!driver) {
        goto err;
    }

    adc = driver->adc_get(fdt, offset, gpio);
    if (!adc) {
        goto err;
    }

    return adc;

err:
    obj_put(gpio);
    return NULL;
}
