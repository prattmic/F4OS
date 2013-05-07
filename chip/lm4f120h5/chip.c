#include <stddef.h>
#include <stdio.h>
#include <dev/resource.h>
#include <kernel/sched.h>

extern resource uart_console;

#if CONFIG_HAVE_ITM && CONFIG_ENABLE_TRACE
extern resource itm_port;
#endif

/* Define default resources */
resource *default_resources[RESOURCE_TABLE_SIZE] = {
    [stdout]    = &uart_console,
    [stderr]    = &uart_console,
#if CONFIG_HAVE_ITM && CONFIG_ENABLE_TRACE
    [TRACEOUT]  = &itm_port,
#endif
};
