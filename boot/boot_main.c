/* Handles the rest of the boot process from where boot.S left off */

#include <stdint.h>
#include <stdio.h>
#include <mm/mm.h>
#include <dev/resource.h>
#include <kernel/sched.h>
#include <kernel/fault.h>

#include <dev/hw/perfcounter.h>
#include <dev/hw/led.h>
#include <dev/hw/usart.h>
#include <dev/hw/usbdev.h>

const char banner[] = "Welcome to...\r\n"
                "\r\n"
                "88888888888      ,d8      ,ad8888ba,     ad88888ba   \r\n"
                "88             ,d888     d8\"\'    `\"8b   d8\"     \"8b  \r\n"
                "88           ,d8\" 88    d8\'        `8b  Y8,          \r\n"
                "88aaaaa    ,d8\"   88    88          88  `Y8aaaaa,    \r\n"
                "88\"\"\"\"\"  ,d8\"     88    88          88    `\"\"\"\"\"8b,  \r\n"
                "88       8888888888888  Y8,        ,8P          `8b  \r\n"
                "88                88     Y8a.    .a8P   Y8a     a8P  \r\n"
                "88                88      `\"Y8888Y\"\'     \"Y88888P\"   \r\n"
                "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";

void os_start(void) __attribute__((section(".kernel")));
void clock(void);
void init_cortex_m(void);

void os_start(void) {
    clock();
    init_cortex_m();

#ifdef CONFIG_PERFCOUNTER
    init_perfcounter();
#endif

#ifdef CONFIG_HAVE_LED
    init_led();
#endif

    init_heap();

#ifdef CONFIG_HAVE_USART
    init_usart();
#endif

#ifdef CONFIG_HAVE_USBDEV
    init_usbdev();
#endif

    printf("\r\n%s\r\n", banner);

    if (default_resources[stdout] != default_resources[stderr]) {
        fprintf(stderr, "\r\n%sStandard error terminal.\r\n", banner);
    }

    start_sched();
    panic_print("Task switching ended.");
}
