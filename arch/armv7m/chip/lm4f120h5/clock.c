#include <arch/chip.h>
#include <arch/chip/registers.h>
#include <arch/chip/rom.h>
#include "sysctl.h"

void init_clock(void) {
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
}
