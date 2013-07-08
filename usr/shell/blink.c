#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arch/chip/registers.h>
#include <dev/hw/led.h>
#include <kernel/sched.h>
#include "app.h"

void led1(void);
void led2(void);
void led3(void);

struct blinker {
    char *name;
    void (*func)(void);
    uint8_t enabled;
} blink_funcs[] = {
    {.name = "1", .func = &led1, .enabled = 0},
    {.name = "2", .func = &led2, .enabled = 0},
    {.name = "3", .func = &led3, .enabled = 0}
};

void blink(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s LED...\r\nExample: blink 2 1\r\n", argv[0]);
        return;
    }

    uint8_t found = 0;

    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < num_leds - 1; j++) {
            if (!strncmp(argv[i], blink_funcs[j].name, 16)) {
                if (blink_funcs[j].enabled) {
                    printf("LED %s already enabled.\r\n", blink_funcs[j].name);
                }
                else {
                    printf("Enabling LED %s...", blink_funcs[j].name);
                    new_task(blink_funcs[j].func, 5, 200*(j+1));
                    blink_funcs[j].enabled = 1;
                    printf("Done.\r\n");
                }

                found = 1;
                continue;
            }
        }

        if (!found) {
            printf("Unknown LED: %s\r\n", argv[i]);
        }
    }
}
DEFINE_APP(blink)

void led1(void) {
    led_toggle(1);
}

void led2(void) {
    led_toggle(2);
}

void led3(void) {
    led_toggle(3);
}
