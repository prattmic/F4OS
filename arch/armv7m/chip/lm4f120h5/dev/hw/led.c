#include <stdint.h>
#include <arch/chip/registers.h>
#include <arch/chip/rom.h>

#include <dev/hw/led.h>

/* GPIO LEDs available */
uint32_t leds_avail[] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

const uint32_t num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);

void init_led(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

    ROM_SysCtlDelay(1);

    /* Set pin 1 (red) to output */
    GPIO_PORTF_DIR_R |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Digital output */
    GPIO_PORTF_DEN_R |= GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

    /* Enable red LED */
    GPIO_PORTF_DATA_R |= GPIO_PIN_1;
}

uint8_t led_enable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R |= leds_avail[led];

    return 0;
}

uint8_t led_disable(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R &= ~leds_avail[led];

    return 0;
}

uint8_t led_toggle(uint32_t led) {
    if (led >= num_leds) {
        return 1;
    }

    GPIO_PORTF_DATA_R ^= leds_avail[led];

    return 0;
}
