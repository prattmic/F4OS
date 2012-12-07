#include <stdint.h>
#include <dev/registers.h>
#include <chip/rom.h>

#include <dev/hw/gpio.h>

/* GPIO LEDs available */
uint32_t gpio_led[] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

uint32_t num_leds = sizeof(gpio_led)/sizeof(gpio_led[0]);

void init_gpio(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

    ROM_SysCtlDelay(1);

    /* Set pin 1 (red) to output */
    GPIO_PORTF_DIR_R |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Digital output */
    GPIO_PORTF_DEN_R |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Enable red LED */
    GPIO_PORTF_DATA_R |= GPIO_PIN_1;
}

uint8_t gpio_enable_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R |= gpio_led[led];

    return 0;
}

uint8_t gpio_disable_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R &= ~gpio_led[led];

    return 0;
}

uint8_t gpio_toggle_led(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R ^= gpio_led[led];

    return 0;
}
