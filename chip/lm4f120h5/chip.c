#include <stddef.h>
#include <stdio.h>
#include <dev/resource.h>
#include <kernel/sched.h>

extern resource uart_console;

/* Define default resources */
resource *default_resources[RESOURCE_TABLE_SIZE] = {
    [stdout] =  &uart_console,
    [stderr] =  &uart_console,
};
