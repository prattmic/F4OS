#ifndef DEV_HW_GPIO_H_INCLUDED
#define DEV_HW_GPIO_H_INCLUDED

#include <stdint.h>

void init_gpio(void) __attribute__((section(".kernel")));

uint8_t gpio_enable_led(uint32_t led);
uint8_t gpio_disable_led(uint32_t led);
uint8_t gpio_toggle_led(uint32_t led);

extern uint32_t num_leds;

#endif
