#include <stdint.h>
#include <dev/registers.h>

#include <dev/hw/led.h>

/* GPIO LEDs available - Red, Green, Orange, Blue */
uint32_t leds_avail[] = {14, 12, 13, 15};

const uint32_t num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);

void init_led(void) {
    /* Enable Port D Clock
    * See docs/stm32f4_ref.pdf page 110 for description of RCC_AHB1ENR */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* Set LED pins to output
    * See docs/stm32f4_ref.pdf page 148 for description of GPIOD_MODER */
    *GPIOD_MODER &= ~(GPIO_MODER_M(12) | GPIO_MODER_M(13) | GPIO_MODER_M(14) | GPIO_MODER_M(15));
    *GPIOD_MODER |= (GPIO_MODER_OUT << GPIO_MODER_PIN(12)) | (GPIO_MODER_OUT << GPIO_MODER_PIN(13)) | (GPIO_MODER_OUT << GPIO_MODER_PIN(14)) | (GPIO_MODER_OUT << GPIO_MODER_PIN(15));

    /* Enable red LED */
    *GPIOD_ODR |= GPIO_ODR_PIN(14);
}

uint8_t led_enable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR |= GPIO_ODR_PIN(leds_avail[led]);

    return 0;
}

uint8_t led_disable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR &= ~(GPIO_ODR_PIN(leds_avail[led]));

    return 0;
}

uint8_t led_toggle(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIOD_ODR ^= GPIO_ODR_PIN(leds_avail[led]);

    return 0;
}
