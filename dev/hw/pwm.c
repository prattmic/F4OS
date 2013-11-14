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
#include <dev/hw/pwm.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <kernel/system.h>
#include <mm/mm.h>

void pwm_dtor(struct obj *o);

struct obj_type pwm_type_s  = {
    .offset = offset_of(struct pwm, obj),
    .dtor = pwm_dtor,
};

struct class pwm_class = INIT_CLASS(pwm_class, "pwm", &pwm_type_s);

void pwm_dtor(struct obj *o) {
    struct pwm *pwm;
    struct pwm_ops *ops;

    assert_type(o, &pwm_type_s);
    pwm = to_pwm(o);
    ops = (struct pwm_ops *)o->ops;
    ops->dtor(pwm);

    /* Decref GPIO obj */
    obj_put(pwm->gpio);

    /* We are completely done with this object, get rid of it */
    collection_del(&pwm_class.instances, o);
    kfree(pwm);
}

int pwm_setup(void) {
    obj_init(&pwm_class.obj, system_class.type, "pwm");
    return 0;
}
CORE_INITIALIZER(pwm_setup)

/*
 * In the event that there is no chip PWM driver, provide stub pwm_get
 * that always returns failure.
 */
struct obj __attribute__((weak)) *pwm_get(struct obj *gpio, uint32_t duty) {
    return NULL;
}
