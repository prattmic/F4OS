#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdint.h>

extern volatile uint32_t system_ticks;

/* Microsecond sleep.  Max precision is system tick period (1/CONFIG_SYSTICK_FREQ). */
int usleep(uint32_t usecs);

#endif
