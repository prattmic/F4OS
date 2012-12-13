#ifndef DEV_HW_LED_H_INCLUDED
#define DEV_HW_LED_H_INCLUDED

#include <stdint.h>

void init_led(void) __attribute__((section(".kernel")));

uint8_t led_enable(uint32_t led);
uint8_t led_disable(uint32_t led);
uint8_t led_toggle(uint32_t led);

extern const uint32_t num_leds;

#endif
