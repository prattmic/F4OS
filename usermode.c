#include "types.h"
#include "registers.h"
#include "task.h"
#include "usermode.h"

void user_mode(void);


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

void led_tasks(void) {
    taskCtrl *blue_led_task;
    taskCtrl *orange_led_task;

    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));

    blue_led_task = create_task(&blue_led, 1, 0);
    orange_led_task = create_task(&orange_led, 1, 0);

    register_task(blue_led_task);
    register_task(orange_led_task);

    blue_led();
    orange_led();
}

void blue_led(void) {
    while (1) {
        uint32_t count = 10000000;

        /* Toggle LED */
        *LED_ODR ^= (1 << 15);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }
    }
}

void orange_led(void) {
    while (1) {
        uint32_t count = 10000000;

        /* Toggle LED */
        *LED_ODR ^= (1 << 13);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }
    }
}
