#include <dev/registers.h>
#include <chip/rom.h>
#include "sysctl.h"

void clock(void) {
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    /* Enable FPU */
    NVIC_CPAC_R = NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL;

    /* Save FPU state */
    NVIC_FPCC_R |= NVIC_FPCC_ASPEN;

    /* Enable bus and usage faults */
    NVIC_SYS_HND_CTRL_R |= NVIC_SYS_HND_CTRL_USAGE | NVIC_SYS_HND_CTRL_BUS;
}
