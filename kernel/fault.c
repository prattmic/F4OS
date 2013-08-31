#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <dev/resource.h>
#include <dev/hw/usart.h>
#include <dev/hw/led.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

#include <kernel/fault.h>

#ifdef CONFIG_HAVE_USART
/*
 * These puts/putc ignore any locking on the resource.
 */
static int printk_puts(rd_t r, char *s) {
    if (usart_ready) {
        return usart_puts(s, NULL);
    }
    else {
        return -1;
    }
}

static int printk_putc(rd_t r, char c) {
    if (usart_ready) {
        return usart_putc(c, NULL);
    }
    else {
        return -1;
    }
}
#else
static int printk_puts(rd_t r, char *s) {return -1;}
static int printk_putc(rd_t r, char c) {return -1;}
#endif

static inline int vprintk(char *fmt, va_list ap) {
    return vfprintf(stderr, fmt, ap, &printk_puts, &printk_putc);
}

int printk(char *fmt, ...) {
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vprintk(fmt, ap);
    va_end(ap);

    return ret;
}

/* Print a message and then panic
 * Accepts standard printf format strings. */
void panic_print(char *fmt, ...) {
    /* Disable interrupts, as the system is going down. */
    disable_interrupts();

#ifdef CONFIG_HAVE_LED
    /* Toggle red LED so there is some indication that something
     * bad has happened if this hangs */
    led_toggle(0);
#endif
    /* We're done here... */
    task_switching = 0;

    /* Print panic message */
    printk("\r\npanic: ");

    va_list ap;
    va_start(ap, fmt);
    vprintk(fmt, ap);
    va_end(ap);

    panic();
}

#ifdef CONFIG_HAVE_LED
void toggle_led_delay(void) {
    uint32_t count = 1000000;

    /* Toggle LED */
    led_toggle(0);

    while (--count) {
        float delay = 2.81;
        delay *= 3.14f;
    }
}
#endif

/*
 * Provide a weak version that does nothing in case
 * the arch doesn't provide one
 */
void __attribute__((weak)) disable_interrupts(void) {}
