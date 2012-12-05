#include <stdint.h>
#include <dev/registers.h>

#include <dev/hw/gpio.h>

/* GPIO LEDs available - Red, Green, Orange, Blue */
uint32_t gpio_led[] = {(1 << 14), (1 << 12), (1 << 13), (1 << 15)};

uint32_t num_leds = sizeof(gpio_led)/sizeof(gpio_led[0]);

void init_gpio(void) {
    /* Enable Port D Clock
    * See docs/stm32f4_ref.pdf page 110 for description of RCC_AHB1ENR */
    *RCC_AHB1ENR |= (1 << 3);

    /* Set LED pins to output
    * See docs/stm32f4_ref.pdf page 148 for description of GPIOD_MODER */
    *GPIOD_MODER |= (1 << (12 * 2)) | (1 << (13 * 2)) | (1 << (14 * 2)) | (1 << (15 * 2));

    /* Enable red LED */
    *GPIOD_ODR |= (1 << 14);
}

uint8_t gpio_enable_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR |= gpio_led[led];

    return 0;
}

uint8_t gpio_disable_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR &= ~gpio_led[led];

    return 0;
}

uint8_t gpio_toggle_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR ^= gpio_led[led];

    return 0;
}
