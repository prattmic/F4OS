#include <arch/chip/gpio.h>
#include <dev/hw/led.h>
#include <board_config.h>

struct lis302dl_accel_config lis302dl_accel_config = {
    .valid = BOARD_CONFIG_VALID_MAGIC,
    .parent_name = "spi1",
    .cs_gpio = STM32F4_GPIO_PE3,
    .cs_active_low = 0,
};

/* GPIO LEDs available - Red, Green, Orange, Blue */
const struct led leds_avail[] = {{STM32F4_GPIO_PD14, 0}, {STM32F4_GPIO_PD12, 0},
                                 {STM32F4_GPIO_PD13, 0}, {STM32F4_GPIO_PD15, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
