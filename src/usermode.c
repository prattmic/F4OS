#include "types.h"
#include "registers.h"
#include "task.h"
#include "mem.h"
#include "systick.h"
#include "usart.h"
#include "semaphore.h"
#include "buddy.h"
#include "interrupt.h"
#include "spi.h"
#include "shell.h"
#include "usermode.h"

struct semaphore faux_sem;

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
    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));

    new_task(&kernel_task, 1, 0);
    new_task(&blue_led, 1, 0);
    new_task(&orange_led, 1, 0);
    new_task(&shell, 1, 0);
    //new_task(&loading, 2, 0);
    //new_task(&usart_echo, 1, 0);
    //new_task(&greedy, 1, 0);

    systick_init();
    start_task_switching();
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

void loading(void) {
    puts("Loading");

    uint8_t num = 3;
    while (num) {
        uint32_t count = 5000000;

        puts(".");

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }

        num--;
    }
    puts("\r\n");
}

void greedy(void) {
    acquire(&faux_sem);
    puts("Grabbed lock\r\n");

    uint8_t num = 6;
    while (num) {
        uint32_t count = 5000000;

        puts(".");

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }

        num--;
    }

    release(&faux_sem);
    puts("Released lock\r\n");
}
