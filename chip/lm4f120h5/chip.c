#include <stddef.h>
#include <dev/resource.h>
#include <kernel/sched.h>
#include <chip/chip.h>

extern resource uart_console;

/* Define default resources */
resource *default_resources[RESOURCE_TABLE_SIZE] = {&uart_console,
                                                    &uart_console,
                                                    NULL};
