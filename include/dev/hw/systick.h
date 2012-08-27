#ifndef DEV_HW_SYSTICK_INCLUDED
#define DEV_HW_SYSTICK_INCLUDED

void init_systick(void) __attribute__((section(".kernel")));

#endif
