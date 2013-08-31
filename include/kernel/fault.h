#ifndef KERNEL_FAULT_H_INCLUDED
#define KERNEL_FAULT_H_INCLUDED

#ifdef DEBUG
#define DEBUG_ON    1
#else
#define DEBUG_ON    0
#endif

#define DEBUG_PRINT(...)    do { if (DEBUG_ON) printk(__VA_ARGS__); } while(0)

extern void panic(void) __attribute__((noreturn));
extern void disable_interrupts(void);

int printk(char *fmt, ...) __attribute__((section(".kernel")));
void panic_print(char *fmt, ...) __attribute__((noreturn));
void toggle_led_delay(void) __attribute__((optimize(0)));

#endif
