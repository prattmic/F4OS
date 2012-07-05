#include "types.h"
#include "registers.h"
#include "task.h"
#include "mem.h"
#include "systick.h"
#include "usart.h"
#include "buddy.h"
#include "usermode.h"

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
    task_ctrl *blue_led_task;
    task_ctrl *orange_led_task;
    task_ctrl *hello_print_task;
    task_ctrl *usart_echo_task;

    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));

    blue_led_task = create_task(&blue_led, 1, 0);
    if (blue_led_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(blue_led_task);
        if (reg_task == NULL) {
            free(blue_led_task->stack_base);
            kfree(blue_led_task);
            puts("Couldn't allocate blue_led_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate blue_led_task, skipping.\r\n");
    }

    orange_led_task = create_task(&orange_led, 1, 0);
    if (orange_led_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(orange_led_task);
        if (reg_task == NULL) {
            free(orange_led_task->stack_base);
            kfree(orange_led_task);
            puts("Couldn't allocate orange_led_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate orange_led_task, skipping.\r\n");
    }

    hello_print_task = create_task(&hello_print, 1, 0);
    if (hello_print_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(hello_print_task);
        if (reg_task == NULL) {
            free(hello_print_task->stack_base);
            kfree(hello_print_task);
            puts("Couldn't allocate hello_print_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate hello_led_task, skipping.\r\n");
    }

    usart_echo_task = create_task(&usart_echo, 1, 0);
    if (usart_echo_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(usart_echo_task);
        if (reg_task == NULL) {
            free(usart_echo_task->stack_base);
            kfree(usart_echo_task);
            puts("Couldn't allocate usart_echo_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate usart_echo_task, skipping.\r\n");
    }

    systick_init();
    start_task_switching();
}

void blue_led(void) {
    while (1) {
        uint32_t count = 9000000;

        puts("Toggling blue LED.\r\n");

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
        uint32_t count = 9000000;

        puts("Toggling orange LED.\r\n");

        /* Toggle LED */
        *LED_ODR ^= (1 << 13);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }
    }
}

void hello_print(void) {
    uint8_t num = 3;
    while (num) {
        uint32_t count = 9000000;

        puts("Hello World.\r\n");

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }

        num--;
    }
    puts("Goodbye World!\r\n");
}
