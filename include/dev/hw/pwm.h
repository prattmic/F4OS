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

#ifndef DEV_PWM_H_INCLUDED
#define DEV_PWM_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>

struct pwm {
    struct obj  *gpio;
    struct obj  obj;
    void        *priv;
};

/* Takes obj and returns containing struct pwm */
static inline struct pwm *to_pwm(struct obj *o) {
    return (struct pwm *) container_of(o, struct pwm, obj);
}

struct pwm_ops {
    /**
     * Enable or disable output
     *
     * Enables or disables PWM output on the channel.
     *
     * All parameters may be get/set whether output is enabled or disabled.
     *
     * @param pwm       PWM output to enable/disable
     * @param enable    Enable output if 1, disable if 0
     * @return zero on success, negative on error
     */
    int     (*enable)(struct pwm *, uint8_t);
    /**
     * Set period
     *
     * Sets the period of the PWM output, in microseconds.  If the
     * period cannot be changed (if a timer is being shared between several
     * outputs, for example), an error value is returned.
     *
     * The period will be set as closely to the passed in parameter as
     * possible.  The actual set period will be returned.
     *
     * @param pwm   PWM output to change period of
     * @param period  Desired period, in microseconds
     * @returns set period, negative on error
     */
    int32_t (*set_period)(struct pwm *, uint32_t);
    /**
     * Get period
     *
     * Gets the current period of the PWM output, in microseconds.
     *
     * If output is disabled, this returns the configured period value,
     * should the output be enabled.
     *
     * @param pwm   PWM output to get period of
     * @returns Period of output
     */
    uint32_t(*get_period)(struct pwm *);
    /**
     * Set pulse width
     *
     * Sets the PWM output pulse width, in microseconds.
     *
     * The pulse width will be set as closely as possible to the passed in
     * parameter.  The actual set pulse width will be returned.
     *
     * @param pwm   PWM output to set
     * @param width Desired pulse width, in microseconds
     * @returns set pulse width, negative on error
     */
    int32_t (*set_pulse_width)(struct pwm *, uint32_t);
    /**
     * Get pulse width
     *
     * Gets the current pulse width of the PWM output, in microseconds.
     *
     * If output is disabled, this returns the configured pulse width value,
     * should the output be enabled.
     *
     * @param pwm   PWM output to get pulse width of
     * @returns Pulse width of output
     */
    uint32_t(*get_pulse_width)(struct pwm *);
    /**
     * Determine if PWM output hardware or software controlled
     *
     * @param pwm   PWM output to check
     * @returns 1 if PWM controlled in hardware (e.g., with a timer),
     *  0 if controlled in software
     */
    uint8_t (*is_hardware)(struct pwm *);
    /**
     * Implementation specific destructor
     *
     * Performs any implementation specific destruction necessary when freeing the
     * PWM object.
     *
     * @param pwm  PWM object being destroyed
     * @returns zero on success, negative on error
     */
    int     (*dtor)(struct pwm *);
};

extern struct class pwm_class;

/**
 * Get PWM object
 *
 * Note: This function is implemented by the chip GPIO driver
 *
 * Get PWM object for passed in GPIO obj.  A PWM object, controlled
 * either in hardware or software is allocated, and will output signals
 * on the passed in GPIO.
 *
 * The desired period is passed in to aid in matching the GPIO with a
 * compatible timer.  If matched with a timer used by other PWM outputs,
 * it may not be possible to change the period after instantiation.
 *
 * If neither a hardware or software object can be allocated, NULL will be
 * returned.
 *
 * The reference count on the GPIO will be incremented when the object is
 * instantiated, and decremented when the object is destroyed.
 *
 * Once allocated, the output must be enabled with the enable method.  Once
 * finish, the object should be freed with pwm_put().
 *
 * @param gpio  GPIO number to get PWM object for
 * @param period  Desired period
 * @returns PWM object on success, NULL on error
 */
struct obj *pwm_get(struct obj *gpio, uint32_t period);

/**
 * Return PWM object
 *
 * Release a PWM object received by pwm_get.  This must be called when
 * you are finished using the PWM.
 *
 * @param obj   PWM obj received from pwm_get to release
 */
static inline void pwm_put(struct obj *o) {
    obj_put(o);
}

#endif
