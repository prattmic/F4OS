#include <stddef.h>
#include <dev/resource.h>
#include <kernel/sched.h>

extern resource usb_console;
extern resource uart_console;

/* Define default resources */
resource *default_resources[RESOURCE_TABLE_SIZE] = {&usb_console,
                                                    &uart_console,
                                                    NULL};
