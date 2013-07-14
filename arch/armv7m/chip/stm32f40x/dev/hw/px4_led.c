#include <stdint.h>
#include <arch/chip/gpio.h>
#include <dev/hw/led.h>

/* GPIO LEDs available - Red, Blue */
const struct led leds_avail[] = {{STM32F4_GPIO_PB14, 0}, {STM32F4_GPIO_PB15, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
