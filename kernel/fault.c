#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <dev/cortex_m.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

#include <dev/resource.h>
#include <dev/hw/usart.h>
#include <dev/hw/led.h>

#include <kernel/fault.h>

void hardfault_handler(void);
void memmanage_handler(void);
void busfault_handler(void);
void usagefault_handler(void);

static inline int vprintk(char *fmt, va_list ap);

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
    __asm__("cpsid  i");
    /* Toggle red LED so there is some indication that something
     * bad has happened if this hangs */
    led_toggle(0);
    /* We're done here... */
    task_switching = 0;
    /* Force release of usart semaphore */
    release(&usart_semaphore);

    /* Print panic message */
    printk("\r\npanic: ");

    va_list ap;
    va_start(ap, fmt);
    vprintk(fmt, ap);
    va_end(ap);

    panic();
}

void toggle_led_delay(void) {
    uint32_t count = 1000000;

    /* Toggle LED */
    led_toggle(0);

    while (--count) {
        float delay = 2.81;
        delay *= 3.14f;
    }
}

void hardfault_handler(void) {
    uint32_t status;
    uint8_t interpretation = 0;

    /* Toggle red LED so there is some indication that something
     * bad has happened if this handler hangs */
    led_toggle(0);
    /* We're done here... */
    task_switching = 0;
    /* Force release of usart semaphore */
    release(&usart_semaphore);

    status = *SCB_HFSR;

    printk("\r\n-----------------Hard Fault-----------------\r\n");
    printk("The hard fault status register contains: 0x%x\r\n", status);
    printk("Interpretation:\r\n");
    
    if (status & SCB_HFSR_VECTTBL) {
        interpretation = 1;
        printk("Vector table hard fault. Indicates a bus fault on a vector" 
             " table read during exception processing.\r\n");
    }
    if (status & SCB_HFSR_FORCED) {
        interpretation = 1;
        printk("Forced hard fault. Indicates a forced hard fault, generated by escalation of a fault"
             " with configurable priority that cannot be handles, either because of priority or because it is"
             " disabled.\r\n");
    }

    if (!interpretation) {
        printk("No idea.  See pg. 226 in the STM32F4 Programming Reference Manual.\r\n");
    }

    panic();
}

void memmanage_handler(void) {
    uint8_t status;
    uint8_t interpretation = 0;

    status = (uint8_t) (*SCB_CFSR & 0xff);

    /* Toggle red LED so there is some indication that something
     * bad has happened if this handler hangs */
    led_toggle(0);
    /* We're done here... */
    task_switching = 0;
    /* Force release of usart semaphore */
    release(&usart_semaphore);

    printk("\r\n-----------------Memory Management Fault-----------------\r\n");
    printk("The memory management fault status register contains: 0x%x\r\n", status);
    if (status & SCB_MMFSR_MMARVALID) {
        printk("Address that caused violation: 0x%x\r\n", *SCB_MMFAR);
    }
    printk("Interpretation:\r\n");

    if (status & SCB_MMFSR_IACCVIOL) {
        interpretation = 1;
        printk("Instruction access violation. This fault occurs on any access to an XN region,"
             " even the MPU is disabled or not present. The PC value stacked for the exception"
             " return points to the faulting instruction.\r\n");
    }
    if (status & SCB_MMFSR_DACCVIOL) {
        interpretation = 1;
        printk("Data access violation flag. The PC value stacked for the"
             " exception return points to the faulting instruction.\r\n");
    }
    if (status & SCB_MMFSR_MUNSTKERR) {
        interpretation = 1;
        printk("Memory manager fault on unstacking for a return from exception. This fault is"
             " chained to the handler. This means that the original return stack is still"
             " present. The processor has not adjusted the SP from the failing return,"
             " and has not performed a new save.\r\n");
    }
    if (status & SCB_MMFSR_MSTKERR) {
        interpretation = 1;
        printk("Memory manager fault on stacking for exception entry. The SP"
             " is still adjusted but the values in the context area on the stack might be incorrect.\r\n");
    }
    if (status & SCB_MMFSR_MLSPERR) {
        interpretation = 1;
        printk("Fault occurred during floating-point lazy state preservation.\r\n");
    }

    if (!interpretation) {
        printk("No idea.  See pg. 225 in the STM32F4 Programming Reference Manual.\r\n");
    }

    panic();
}

void busfault_handler(void) {
    uint8_t status;
    uint8_t interpretation = 0;

    status = (uint8_t) ((*SCB_CFSR >> 8) & 0xff);

    /* Toggle red LED so there is some indication that something
     * bad has happened if this handler hangs */
    led_toggle(0);
    /* We're done here... */
    task_switching = 0;
    /* Force release of usart semaphore */
    release(&usart_semaphore);

    printk("\r\n-----------------Bus Fault-----------------\r\n");
    printk("The bus fault status register contains: 0x%x\r\n", status);
    if (status & SCB_BFSR_BFARVALID) {
        printk("Address that caused exception: 0x%x\r\n", *SCB_BFAR);
    }
    printk("Interpretation:\r\n");
    
    if (status & SCB_BFSR_IBUSERR) {
        interpretation = 1;
        printk("Instruction bus error. The processor detects the instruction bus error on prefetching"
             " an instruction, but it sets this flag only if it attempts to issue the faulting"
             " instruction.\r\n");
    }
    if (status & SCB_BFSR_PRECISERR) {
        interpretation = 1;
        printk("Precise data bus error.\r\n");
    }
    if (status & SCB_BFSR_IMPRECISERR) {
        interpretation = 1;
        printk("Imprecise data bus error. This is an asynchronous fault. Therefore, if it is detected"
             " when the priority of the current process is higher than the bus fault priority, the bus fault"
             " becomes pending and becomes active only when the processor returns from all higher priority"
             " processes. If a precise fault occurs before the processor enters the handler for the imprecise"
             " bus fault, the handler detects both IMPRECISERR set to 1 and one of the precise fault status"
             " bits set to 1.\r\n");
    }
    if (status & SCB_BFSR_UNSTKERR) {
        interpretation = 1;
        printk("Bus fault on unstacking for a return from exception. This fault is chained to the"
             " handler. The original return stack is still present."
             " The processor does not adjust the SP from the failing return.\r\n");
    }
    if (status & SCB_BFSR_STKERR) {
        interpretation = 1;
        printk("Bus fault on stacking for exception entry. The SP is still adjusted but the values"
             " in the context area on the stack might be incorrect.\r\n");
    }
    if (status & SCB_BFSR_LSPERR) {
        interpretation = 1;
        printk("A bus fault occurred during floating-point lazy state preservation.\r\n");
    }

    if (!interpretation) {
        printk("No idea.  See pg. 224 in the STM32F4 Programming Reference Manual.\r\n");
    }

    panic();
}

void usagefault_handler(void) {
    uint16_t status;
    uint8_t interpretation = 0;

    status = (uint16_t) (*SCB_CFSR >> 16);

    /* Toggle red LED so there is some indication that something
     * bad has happened if this handler hangs */
    led_toggle(0);
    /* We're done here... */
    task_switching = 0;
    /* Force release of usart semaphore */
    release(&usart_semaphore);

    printk("\r\n-----------------Usage Fault-----------------\r\n");
    printk("The usage fault status register contains: 0x%x\r\n", status);
    printk("Interpretation:\r\n");
    
    if (status & SCB_UFSR_UNDEFINSTR) {
        interpretation = 1;
        printk("Undefined instruction usage fault. The PC value"
             " stacked for the exception return points to the undefined instruction.\r\n");
    }
    if (status & SCB_UFSR_INVSTATE) {
        interpretation = 1;
        printk("Invalid state usage fault. The PC value stacked for the"
             " exception return points to the instruction that attempted the illegal use of the EPSR.\r\n");
    }
    if (status & SCB_UFSR_INVPC) {
        interpretation = 1;
        printk("Invalid PC load usage fault, caused by an invalid PC load by EXC_RETURN.\r\n");
    }
    if (status & SCB_UFSR_NOCP) {
        interpretation = 1;
        printk("No coprocessor usage fault. The processor does not support coprocessor instructions\r\n");
    }
    if (status & SCB_UFSR_UNALIGNED) {
        interpretation = 1;
        printk("Unaligned access usage fault. Unaligned LDM, STM, LDRD, and STRD instructions"
             " always fault irrespective of the setting of UNALIGN_TRP.\r\n");
    }
    if (status & SCB_UFSR_DIVBYZERO) {
        interpretation = 1;
        printk("Divide by zero usage fault. The PC value stacked for the exception return"
             " points to the instruction that performed the divide by zero.\r\n");
    }

    if (!interpretation) {
        printk("No idea.  See pg. 223 in the STM32F4 Programming Reference Manual.\r\n");
    }

    panic();
}
