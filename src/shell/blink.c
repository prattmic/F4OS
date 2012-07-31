#include "types.h"
#include "registers.h"
#include "string.h"
#include "task.h"
#include "semaphore.h"
#include "usart.h"
#include "blink.h"

uint8_t enabled_blue = 0;
uint8_t enabled_orange = 0;

void blink(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s LED...\r\nExample: blink blue orange\r\n", argv[0]);
        return;
    }

    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "blue", 16)) {
            if (enabled_blue) {
                printf("Blue LED already enabled.\r\n");
            }
            else {
                printf("Enabling blue LED...");
                new_task(&blue_led, 1, 0);
                enabled_blue = 1;
                printf("Done.\r\n");
            }
        }
        else if (!strncmp(argv[i], "orange", 16)) {
            if (enabled_orange) {
                printf("Orange LED already enabled.\r\n");
            }
            else {
                printf("Enabling orange LED...");
                new_task(&orange_led, 1, 0);
                enabled_orange = 1;
                printf("Done.\r\n");
            }
        }
        else {
            printf("Unknown LED: %s\r\n", argv[i]);
        }
    }
}

void blue_led(void) {
    while (1) {
        uint32_t count = 9000000;

        //puts("Toggling blue LED.\r\n");

        /* Toggle LED */
        *LED_ODR ^= (1 << 15);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }
    }
}

void orange_led(void) {
    /* uint8_t i = 1; */

    while (1) {
        uint32_t count = 9000000;

        //puts("Toggling orange LED.\r\n");

        /* Toggle LED */
        *LED_ODR ^= (1 << 13);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }

        /* Testing priority inheritance with greedy() */
        /* if (i) {
            k_curr_task->task->priority = 2;
            remove_task(k_curr_task);
            append_task(k_curr_task);
            acquire(&faux_sem);
            puts("Orange got the lock!\r\n");
            release(&faux_sem);
            k_curr_task->task->priority = 1;
            remove_task(k_curr_task);
            append_task(k_curr_task);
            i = 0;
        } */
    }
}
