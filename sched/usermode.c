#include "types.h"
#include "registers.h"
#include "task.h"
#include "mem.h"
#include "systick.h"
#include "stdio.h"
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

void start_tasks(void) {
    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));
    new_task(&kernel_task, 1, 0);
    new_task(&shell, 1, 0);
    //new_task(&loading, 2, 0);
    //new_task(&usart_echo, 1, 0);
    //new_task(&greedy, 1, 0);
    systick_init();
    start_task_switching();
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
