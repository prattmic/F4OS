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
    task_ctrl *kernel_action_task;
    task_ctrl *blue_led_task;
    task_ctrl *orange_led_task;
    task_ctrl *loading_task;
    //task_ctrl *usart_echo_task;
    task_ctrl *shell_task;
    task_ctrl *greedy_task;

    /* Enable blue and orange LEDs */
    *GPIOD_MODER |= (1 << (13 * 2)) | (1 << (15 * 2));

    kernel_action_task = create_task(&kernel_task, 1, 0);
    if (kernel_action_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(kernel_action_task);
        if (reg_task == NULL) {
            free(kernel_action_task->stack_base);
            kfree(kernel_action_task);
            puts("Couldn't allocate kernel_action_task, panicing.\r\n");
            panic();
        }
    }
    else {
        puts("Couldn't allocate kernel_action_task, panicing.\r\n");
        panic();
    }

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

    loading_task = create_task(&loading, 2, 0);
    if (loading_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(loading_task);
        if (reg_task == NULL) {
            free(loading_task->stack_base);
            kfree(loading_task);
            puts("Couldn't allocate loading_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate hello_led_task, skipping.\r\n");
    }

    //usart_echo_task = create_task(&usart_echo, 1, 0);
    //if (usart_echo_task != NULL) {
    //    task_node *reg_task;

    //    reg_task = register_task(usart_echo_task);
    //    if (reg_task == NULL) {
    //        free(usart_echo_task->stack_base);
    //        kfree(usart_echo_task);
    //        puts("Couldn't allocate usart_echo_task, skipping.\r\n");
    //    }
    //}
    //else {
    //    puts("Couldn't allocate usart_echo_task, skipping.\r\n");
    //}

    shell_task = create_task(&shell, 1, 0);
    if (shell_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(shell_task);
        if (reg_task == NULL) {
            free(shell_task->stack_base);
            kfree(shell_task);
            puts("Couldn't allocate shell_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate shell_task, skipping.\r\n");
    }

    greedy_task = create_task(&greedy, 1, 0);
    if (greedy_task != NULL) {
        task_node *reg_task;

        reg_task = register_task(greedy_task);
        if (reg_task == NULL) {
            free(greedy_task->stack_base);
            kfree(greedy_task);
            puts("Couldn't allocate greedy_task, skipping.\r\n");
        }
    }
    else {
        puts("Couldn't allocate greedy_task, skipping.\r\n");
    }

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
    uint8_t i = 1;

    while (1) {
        uint32_t count = 9000000;

        //puts("Toggling orange LED.\r\n");

        /* Toggle LED */
        *LED_ODR ^= (1 << 13);

        while (--count) {
            float delay = 2.81;
            delay *= 3.14f;
        }

        if (i) {
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
        }
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
