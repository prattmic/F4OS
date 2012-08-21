#ifndef KERNEL_FAULT_H_INCLUDED
#define KERNEL_FAULT_H_INCLUDED

extern void panic(void);

void panic_print(char *s);
void toggle_led_delay(void) __attribute__((optimize(0)));

#endif
