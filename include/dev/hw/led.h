#ifndef DEV_HW_LED_H_INCLUDED
#define DEV_HW_LED_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>

struct led {
    uint32_t gpio;
    uint8_t active_low;
};

/**
 * Enable power indication LED
 *
 * If LED cannot be initialized, the function returns as normal
 */
void init_power_led(void) __attribute__((section(".kernel")));

/**
 * Get LED object instance
 *
 * @param num   LED number to get instance of
 * @returns LED obj instance, NULL on failure
 */
struct obj *led_get(uint8_t num);

/**
 * Put LED object instance
 *
 * Return an LED object instance when done with it
 *
 * @param led   LED object to put
 */
void led_put(struct obj *led);

/**
 * Set LED output value
 *
 * @param led   LED object to set
 * @param value 1 to enable LED, 0 to disable LED
 * @returns 0 on success, negative on error
 */
int led_set(struct obj *led, uint8_t value);

/**
 * Enable LED
 *
 * @param led   LED object to enable
 * @returns zero on success, negative on error
 */
int led_enable(struct obj *led);

/**
 * Disable LED
 *
 * @param led   LED object to disable
 * @returns zero on success, negative on error
 */
int led_disable(struct obj *led);

/**
 * Toggle LED
 *
 * Switch LED from enabled to disabled or disable to enabled
 *
 * @param led   LED object to toggle
 * @returns zero on success, negative on error
 */
int led_toggle(struct obj *led);

/* Provided in a board-specific file */
extern const struct led leds_avail[];
extern const int num_leds;

#endif
