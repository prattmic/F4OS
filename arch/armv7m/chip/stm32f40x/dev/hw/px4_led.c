#include <stdint.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>

#include <dev/hw/led.h>

/* GPIO LEDs available - Red, Blue */
uint32_t leds_avail[] = {14, 15};

const uint32_t num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);

void init_led(void) {
    /* Enable Port B Clock
    * See docs/stm32f4_ref.pdf page 110 for description of RCC_AHB1ENR */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* Set LED pins to output
    * See docs/stm32f4_ref.pdf page 148 for description of GPIOB_MODER */

    /* PB14 */
    gpio_moder(GPIOB, 14, GPIO_MODER_OUT);

    /* PB15 */
    gpio_moder(GPIOB, 15, GPIO_MODER_OUT);
    led_disable(1);

    /* Enable red LED */
    led_enable(0);
}

uint8_t led_enable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    /* PX4 LEDs are active low */
    *GPIO_ODR(GPIOB) &= ~(GPIO_ODR_PIN(leds_avail[led]));

    return 0;
}

uint8_t led_disable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    /* PX4 LEDs are active low */
    *GPIO_ODR(GPIOB) |= GPIO_ODR_PIN(leds_avail[led]);

    return 0;
}

uint8_t led_toggle(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    *GPIO_ODR(GPIOB) ^= GPIO_ODR_PIN(leds_avail[led]);

    return 0;
}
