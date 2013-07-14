#include <arch/chip/gpio.h>
#include <dev/hw/led.h>

/* GPIO LEDs available - Red, Green, Orange, Blue */
const struct led leds_avail[] = {{STM32F4_GPIO_PD14, 0}, {STM32F4_GPIO_PD12, 0},
                                 {STM32F4_GPIO_PD13, 0}, {STM32F4_GPIO_PD15, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
