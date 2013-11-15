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

#ifndef DEV_GPIO_H_INCLUDED
#define DEV_GPIO_H_INCLUDED

#include <stdint.h>
#include <kernel/class.h>
#include <kernel/obj.h>

struct gpio {
    int         num;
    uint8_t     active_low;
    struct obj  obj;
};

/* Takes obj and returns containing struct gpio */
static inline struct gpio *to_gpio(struct obj *o) {
    return (struct gpio *) container_of(o, struct gpio, obj);
}

enum gpio_direction {
    GPIO_INPUT  = (0 << 0),
    GPIO_OUTPUT = (1 << 0),
    GPIO_DIRECTION_MASK = (1 << 0),
};

enum gpio_error {
    GPIO_OK = 0,            /* No error */
    GPIO_ERR_INVAL,         /* GPIO value does not coorespond to a valid GPIO */
    GPIO_ERR_UNAVAIL,       /* GPIO is currently unavailable */
    GPIO_ERR_DIR_UNSUPPT,   /* Direction unsupported for this GPIO */
    GPIO_ERR_FLG_UNSUPPT,   /* Unsupported implementation flag */
};

struct gpio_ops {
    /**
     * Reset GPIO to low power state
     *
     * Resets the GPIO to a low power state, as determined by the GPIO
     * implementation.  There are no direction, value, or mode guarantees
     * for the GPIO once this function is called.
     *
     * Generally called when the GPIO is no longer needed.
     *
     * @param gpio  GPIO to reset
     * @returns zero on success, negative on error (one of enum gpio_error)
     */
    int     (*reset)(struct gpio *);
    /**
     * Set active low configuration
     *
     * Configures the GPIO to behave active low or active high.  An active
     * high output is set to the high state when a non-zero value is written.
     * Similarly, an active high input returns a 1 when the GPIO is in the
     * high state.
     *
     * An active low GPIO does the opposite, setting the output to the low
     * state when a non-zero value is written, and returning 1 for low inputs.
     *
     * Thus, with the appropriate active low setting, the input and output values
     * can be treated as "on" and "off" rather than specifically refering to
     * the state of the pin.
     *
     * The GPIO resets to an active high configuration.  Newly opened GPIOs
     * will be configured active high.
     *
     * @param gpio          GPIO to configure
     * @param active_low    1=Active low    0=Active high
     * @returns zero on success, negative on error
     */
    int     (*active_low)(struct gpio *, int);
    /**
     * Set GPIO direction
     *
     * Set direction of GPIO (input or output).  The universal input and
     * output flags, defined in enum gpio_direction, will always be accepted
     * by the driver.  Optionally, driver dependent flags may optionally be
     * implemented in the driver.  Drivers that do not support additional
     * flags must ignore them.
     *
     * @param gpio  GPIO to set
     * @param flags Direction flags. Values defined in enum gpio_direction.
     *              Additonal flags may be implemented by the driver.
     * @returns zero on success, negative on error (one of enum gpio_error)
     */
    int     (*direction)(struct gpio *, int);
    /**
     * Get GPIO input value
     *
     * Get binary value from GPIO configured as input.
     *
     * Active low state of the GPIO is taken into account.  When active high,
     * if GPIO is in the high state, 1 is returned, else 0 is returned.
     * When active low, the opposite is true.
     *
     * Behavior is implementation defined for GPIOs configured as an output.
     *
     * @param gpio  GPIO to get value from
     * @returns Value from input, implementation defined from output
     */
    unsigned int    (*get_input_value)(struct gpio *);
    /**
     * Set GPIO output value
     *
     * Set binary value for GPIO configured as output.
     *
     * Active low state of the GPIO is taken into account.  When active high,
     * a non-zero value will set the GPIO to the high state.  A zero value will
     * set the GPIO to the low state.  When active low, the opposite is true.
     *
     * Behavior is implementation defined for GPIOs configured as an input.
     *
     * @param gpio  GPIO to set value of
     * @param value Value to set
     * @returns zero on success, negative on error (one of enum gpio_error)
     */
    int     (*set_output_value)(struct gpio *, unsigned int);
    /**
     * Get GPIO output value
     *
     * Get binary value configured for GPIO output.
     *
     * Active low state of the GPIO is taken into account.  When active high,
     * A non-zero value indicates that the GPIO is configured to the high
     * state.  A zero value indicates that the GPIO is configured to the low
     * state.  When active low, the opposite is true.
     *
     * @param gpio  GPIO to set value of
     * @returns Configured output value
     */
    unsigned int    (*get_output_value)(struct gpio *);
    /**
     * Set GPIO implementation defined value
     *
     * This function is entirely implementation defined.  It takes flags and a value,
     * generally used to describe a value to get or set.
     *
     * Generally used for setting driver specific fields, such as pin pull-up/down,
     * speed, alternative functions, etc.
     *
     * @param flags Implementation defined, generally describes a field to operate on
     * @param value Implementation defined, generally the value to set a field to
     * @returns Implementation defined, generally error value, or field value
     */
    int     (*set_flags)(struct gpio *, unsigned int, int);
    /**
     * Implementation specific destructor
     *
     * Performs any implementation specific destruction necessary when freeing the
     * GPIO object.
     *
     * @param gpio  GPIO object being destroyed
     * @returns zero on success, negative on error
     */
    int     (*dtor)(struct gpio *);
};

extern struct class gpio_class;

/**
 * Determine GPIO validity
 *
 * Determines if a given GPIO number cooresponds to a valid GPIO
 *
 * Note: This function is implemented by the chip GPIO driver
 *
 * @param gpio  GPIO number to determine availability of
 * @return 0 if valid, else -GPIO_INVAL
 */
int gpio_valid(uint32_t gpio);

/**
 * Determine GPIO availability
 *
 * Determines if a given GPIO is available to get
 *
 * @param gpio  GPIO number to determine availability of
 * @return 0 if available, else -GPIO_INVAL or -GPIO_UNAVAIL
 */
int gpio_available(uint32_t gpio);

/**
 * Get GPIO object
 *
 * Get GPIO object for passed in GPIO number.  Call gpio_available
 * to determine if a GPIO is valid and available.
 *
 * @param gpio  GPIO number to get object for
 * @returns GPIO object on success, NULL on error
 */
struct obj *gpio_get(uint32_t gpio);

/**
 * Return GPIO object
 *
 * Release a GPIO object received by gpio_get.  This must be called when
 * you are finished using the GPIO.
 *
 * @param obj   GPIO obj received from get_get to release
 */
static inline void gpio_put(struct obj *o) {
    obj_put(o);
}

/* Private functions */

/**
 * Instantiate GPIO object
 *
 * Note: This function is implemented by the chip GPIO driver
 *
 * This private function instantiates the actual GPIO function,
 * settings the appropriate ops for the given GPIO.  It does not
 * need to set any fields in struct gpio.
 *
 * @param gpio  GPIO number to instantiate
 * @return pointer to obj on success, NULL on error
 */
struct obj *_gpio_instantiate(uint32_t gpio);

#endif
