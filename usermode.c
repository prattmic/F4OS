#include "types.h"
#include "registers.h"

void user_mode(void);

void unprivileged_test(void);
void toggle_led_delay(void);

void unprivileged_test(void) {
    int test = 0;

    test += 1;

    /* Enter user mode */
    user_mode();

    test += 1;  /* It should fail here */
}

void toggle_led_delay(void) {
    uint32_t count = 10000000;

    /* Toggle LED */
    *LED_ODR ^= (1 << 14);

    while (--count) {
        float delay = 2.81;
        delay *= 3.14f;
    }
}
