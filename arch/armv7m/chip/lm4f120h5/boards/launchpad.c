#include <arch/chip/gpio.h>
#include <dev/hw/led.h>

/* GPIO LEDs available */
const struct led leds_avail[] = {{LM4F_GPIO_PF1, 0}, {LM4F_GPIO_PF2, 0},
                                 {LM4F_GPIO_PF3, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
