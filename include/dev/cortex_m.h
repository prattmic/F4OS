#ifndef DEV_CORTEX_M_H_INCLUDED
#define DEV_CORTEX_M_H_INCLUDED

#include <stdint.h>

/* Tells the compiler to provide the value in reg as
 * an input to an inline assembly block.  Even though
 * the block contains no instructions, the compiler may
 * not optimize it away, and is told that the value may
 * change and should not be relied upon. */
inline void READ_AND_DISCARD(volatile uint32_t *reg) __attribute__((always_inline));
inline void READ_AND_DISCARD(volatile uint32_t *reg) {
    asm volatile ("" : "=m" (*reg) : "r" (*reg));
}

inline uint8_t FAULTMASK(void) __attribute__((always_inline));
inline uint8_t FAULTMASK(void) {
    uint8_t val;

    asm("mrs    %[val], faultmask"
        :[val] "=r" (val)
        ::);

    return val;
}

inline uint8_t IPSR(void) __attribute__((always_inline));
inline uint8_t IPSR(void) {
    uint8_t val;

    asm("mrs    %[val], ipsr"
        :[val] "=r" (val)
        ::);

    return val;
}

inline uint32_t *PSP(void) __attribute__((always_inline));
inline uint32_t *PSP(void) {
    uint32_t *val;

    asm("mrs    %[val], psp"
        :[val] "=r" (val)
        ::);

    return val;
}

inline uint32_t *MSP(void) __attribute__((always_inline));
inline uint32_t *MSP(void) {
    uint32_t *val;

    asm("mrs    %[val], msp"
        :[val] "=r" (val)
        ::);

    return val;
}

#define CORESIGHT_LOCK_MAGIC            (0xC5ACCE55)                                            /* Magic used to enable access to CoreSight components */

/* Cortex M4 General Registers */

/* System Control Map */
#define ITM_BASE                        (uint32_t) (0xE0000000)                                 /* Instrumental Trace Macrocell Base Address */
#define DWT_BASE                        (uint32_t) (0xE0001000)                                 /* Data Watchpoint and Trace Base Address */
#define SCS_BASE                        (uint32_t) (0xE000E000)                                 /* System Control Space Base Address */
#define SYSTICK_BASE                    (SCS_BASE + 0x0010)                                     /* Systick Registers Base Address */
#define NVIC_BASE                       (SCS_BASE + 0x0100)                                     /* Nested Vector Interrupt Control */
#define SCB_BASE                        (SCS_BASE + 0x0D00)                                     /* System Control Block Base Address */
#define MPU_BASE                        (SCB_BASE + 0x0090)                                     /* MPU Block Base Address */
#define DEBUG_BASE                      (SCB_BASE + 0x00F0)                                     /* Debug Base Address */
#define FPU_BASE                        (SCB_BASE + 0x0230)                                     /* FPU Block Base Address */
#define TPIU_BASE                       (uint32_t) (0xE0040000)                                 /* Trace Point Interface Unit Base Address */

/* Instrumental Trace Macrocell (ITM) */
#define ITM_STIM(n)                     (volatile uint32_t *) (ITM_BASE + 4*n)                  /* Stimulus (data) Port n */
#define ITM_TER(n)                      (volatile uint32_t *) (ITM_BASE + 0xE00 + 4*n)          /* Trace enable register n */
#define ITM_TPR                         (volatile uint32_t *) (ITM_BASE + 0xE40)                /* Trace privilege register */
#define ITM_TCR                         (volatile uint32_t *) (ITM_BASE + 0xE80)                /* Trace control register */
#define ITM_LAR                         (volatile uint32_t *) (ITM_BASE + 0xFB0)                /* Lock access register */
#define ITM_LSR                         (volatile uint32_t *) (ITM_BASE + 0xFB4)                /* Lock status register */

/* SysTick Timer */
#define SYSTICK_CTL                     (volatile uint32_t *) (SYSTICK_BASE)                    /* Control register for SysTick timer peripheral */
#define SYSTICK_RELOAD                  (volatile uint32_t *) (SYSTICK_BASE + 0x04)             /* Value assumed by timer upon reload */
#define SYSTICK_VAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x08)             /* Current value of timer */
#define SYSTICK_CAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x0C)             /* Calibration settings/value register */

/* Nested Vector Interrupt Controller */
#define NVIC_ISER0                      (volatile uint32_t *) (NVIC_BASE + 0x000)               /* Interrupt set-enable register 0 */
#define NVIC_ISER1                      (volatile uint32_t *) (NVIC_BASE + 0x004)               /* Interrupt set-enable register 1 */
#define NVIC_ISER2                      (volatile uint32_t *) (NVIC_BASE + 0x008)               /* Interrupt set-enable register 2 */
#define NVIC_ISER3                      (volatile uint32_t *) (NVIC_BASE + 0x00C)               /* Interrupt set-enable register 3 */
#define NVIC_ICER0                      (volatile uint32_t *) (NVIC_BASE + 0x080)               /* Interrupt clear-enable register 0 */
#define NVIC_ICER1                      (volatile uint32_t *) (NVIC_BASE + 0x084)               /* Interrupt clear-enable register 1 */
#define NVIC_ICER2                      (volatile uint32_t *) (NVIC_BASE + 0x088)               /* Interrupt clear-enable register 2 */
#define NVIC_ISPR0                      (volatile uint32_t *) (NVIC_BASE + 0x100)               /* Interrupt set-pending register 0 */
#define NVIC_ICPR0                      (volatile uint32_t *) (NVIC_BASE + 0x180)               /* Interrupt clear-pending register 0 */
#define NVIC_ICPR1                      (volatile uint32_t *) (NVIC_BASE + 0x184)               /* Interrupt clear-pending register 1 */
#define NVIC_ICPR2                      (volatile uint32_t *) (NVIC_BASE + 0x188)               /* Interrupt clear-pending register 2 */
#define NVIC_IPR(n)                     (volatile uint8_t *)  (NVIC_BASE + 0x300 + n)           /* Interrupt n priority register */

/* System Control Block (SCB) */
#define SCB_ICSR                        (volatile uint32_t *) (SCB_BASE + 0x004)                /* Interrupt Control and State Register */
#define SCB_VTOR                        (volatile uint32_t *) (SCB_BASE + 0x008)                /* Vector Table Offset Register */
#define SCB_SCR                         (volatile uint32_t *) (SCB_BASE + 0x010)                /* System Control Register */
#define SCB_SHCSR                       (volatile uint32_t *) (SCB_BASE + 0x024)                /* System Handler Control and State Register */
#define SCB_CFSR                        (volatile uint32_t *) (SCB_BASE + 0x028)                /* Configurable fault status register - Describes Usage, Bus, and Memory faults */
#define SCB_HFSR                        (volatile uint32_t *) (SCB_BASE + 0x02C)                /* Hard fault status register - Describes hard fault */
#define SCB_MMFAR                       (volatile uint32_t *) (SCB_BASE + 0x034)                /* Memory management fault address register - Address that caused fault */
#define SCB_BFAR                        (volatile uint32_t *) (SCB_BASE + 0x038)                /* Bus fault address register - Address that caused fault */
#define SCB_CPACR                       (volatile uint32_t *) (SCB_BASE + 0x088)                /* Coprocessor (FPU) Access Control Register */

/* Memory Protection Unit (MPU)
 * ST PM0214 (Cortex M4 Programming Manual) pg. 195 */
#define MPU_TYPER                       (volatile uint32_t *) (MPU_BASE + 0x00)                 /* MPU Type Register - Describes HW MPU */
#define MPU_CTRL                        (volatile uint32_t *) (MPU_BASE + 0x04)                 /* MPU Control Register */
#define MPU_RNR                         (volatile uint32_t *) (MPU_BASE + 0x08)                 /* MPU Region Number Register */
#define MPU_RBAR                        (volatile uint32_t *) (MPU_BASE + 0x0C)                 /* MPU Region Base Address Register */
#define MPU_RASR                        (volatile uint32_t *) (MPU_BASE + 0x10)                 /* MPU Region Attribute and Size Register */

/* Debug */
#define DEBUG_DHCSR                     (volatile uint32_t *) (DEBUG_BASE + 0x00)               /* Debug Halting Control and Status Register */
#define DEBUG_DEMCR                     (volatile uint32_t *) (DEBUG_BASE + 0x0C)               /* Debug Exception Monitor and Control Register */

/* Floating Point Unit (FPU)
 * ST PM0214 (Cortex M4 Programming Manual) pg. 236 */
#define FPU_CCR                         (volatile uint32_t *) (FPU_BASE + 0x04)                 /* FPU Context Control Register */
#define FPU_CAR                         (volatile uint32_t *) (FPU_BASE + 0x08)                 /* FPU Context Address Register */

/* Trace Port Interface Unit (TPIU) */
#define TPIU_SSPSR                      (volatile uint32_t *) (TPIU_BASE + 0x000)               /* TPIU Supported Parallel Port Sizes Register */
#define TPIU_CSPSR                      (volatile uint32_t *) (TPIU_BASE + 0x004)               /* TPIU Current Parallel Port Size Register */
#define TPIU_ACPR                       (volatile uint32_t *) (TPIU_BASE + 0x010)               /* TPIU Asynchronous Clock Prescaler Register */
#define TPIU_SPPR                       (volatile uint32_t *) (TPIU_BASE + 0x0F0)               /* TPIU Selected Pin Protocol Register */
#define TPIU_FFCR                       (volatile uint32_t *) (TPIU_BASE + 0x304)               /* TPIU Formatter and Flush Control Register */
#define TPIU_TYPE                       (volatile uint32_t *) (TPIU_BASE + 0xFC8)               /* TPIU Type Register */

/**********************************************************************************************************************************************/

/* Instrumental Trace Macrocell (ITM) */
#define ITM_TCR_ITMENA                  (uint32_t) (1 << 0)                                     /* ITM Enable */
#define ITM_TCR_TSENA                   (uint32_t) (1 << 1)                                     /* Timestamp generation enable */
#define ITM_TCR_SYNCENA                 (uint32_t) (1 << 2)                                     /* Sync packet transmission enable */
#define ITM_TCR_TXENA                   (uint32_t) (1 << 3)                                     /* Packet forwarding to TPIU enable */
#define ITM_TCR_SWOENA                  (uint32_t) (1 << 4)                                     /* Timestamp asynchronous clocking enable */
#define ITM_TCR_TSPRSC_MASK             (uint32_t) ((1 << 8) | (1 << 9))                        /* Timestamp prescale mask */
#define ITM_TCR_GTSFREQ_MASK            (uint32_t) ((1 << 10) | (1 << 11))                      /* Global timestamp frequency mask */
#define ITM_TCR_BUSID(x)                (uint32_t) (x << 16)                                    /* Trace bus ID */
#define ITM_TCR_BUSY                    (uint32_t) (1 << 23)                                    /* ITM Busy */

/* System Control Block */
#define SCB_ICSR_PENDSVCLR              (uint32_t) (1 << 27)                                    /* Clear PendSV interrupt */
#define SCB_ICSR_PENDSVSET              (uint32_t) (1 << 28)                                    /* Set PendSV interrupt */

#define SCB_SCR_SLEEPONEXIT             (uint32_t) (1 << 1)                                     /* Sleep on return from interrupt routine */
#define SCB_SCR_SLEEPDEEP               (uint32_t) (1 << 2)                                     /* Use deep sleep as low power mode */
#define SCB_SCR_SEVONPEND               (uint32_t) (1 << 4)                                     /* Send event on pending exception */

#define SCB_SHCSR_MEMFAULTENA           (uint32_t) (1 << 16)                                    /* Enables Memory Management Fault */
#define SCB_SHCSR_BUSFAULTENA           (uint32_t) (1 << 17)                                    /* Enables Bus Fault */
#define SCB_SHCSR_USEFAULTENA           (uint32_t) (1 << 18)                                    /* Enables Usage Fault */

/* Hard Fault Status Register */
#define SCB_HFSR_VECTTBL                (uint32_t) (1 << 1)                                     /* Vector table hard fault.  Bus fault on vector table read during exception handling. */
#define SCB_HFSR_FORCED                 (uint32_t) (1 << 30)                                    /* Forced hard fault.  Escalation of another fault. */

/* Memory Management Fault Status Register */
#define SCB_MMFSR_IACCVIOL              (uint8_t)  (1 << 0)                                     /* Instruction access violation.  No address in MMFAR */
#define SCB_MMFSR_DACCVIOL              (uint8_t)  (1 << 1)                                     /* Data access violation.  Address in MMFAR */
#define SCB_MMFSR_MUNSTKERR             (uint8_t)  (1 << 3)                                     /* Fault on unstacking from exception.  No address in MMAR */
#define SCB_MMFSR_MSTKERR               (uint8_t)  (1 << 4)                                     /* Fault on stacking for exception.  No address in MMFAR */
#define SCB_MMFSR_MLSPERR               (uint8_t)  (1 << 5)                                     /* Fault during FP lazy state preservation. */
#define SCB_MMFSR_MMARVALID             (uint8_t)  (1 << 7)                                     /* MMFAR holds valid address */

/* Bus Fault Status Register */
#define SCB_BFSR_IBUSERR                (uint8_t)  (1 << 0)                                     /* Instruction bus error.  No address in BFAR */
#define SCB_BFSR_PRECISERR              (uint8_t)  (1 << 1)                                     /* Precise data bus error.  Address in BFAR */
#define SCB_BFSR_IMPRECISERR            (uint8_t)  (1 << 2)                                     /* Imprecise data bus error.  No address in BFAR */
#define SCB_BFSR_UNSTKERR               (uint8_t)  (1 << 3)                                     /* Fault on unstacking from exception.  No address in BFAR */
#define SCB_BFSR_STKERR                 (uint8_t)  (1 << 4)                                     /* Fault on stacking for exception.  No address in BFAR */
#define SCB_BFSR_LSPERR                 (uint8_t)  (1 << 5)                                     /* Fault on FP lazy state preservation. */
#define SCB_BFSR_BFARVALID              (uint8_t)  (1 << 7)                                     /* BFAR holds valid address */

/* Usage Fault Status Register */
#define SCB_UFSR_UNDEFINSTR             (uint16_t) (1 << 0)                                     /* Undefined instruction */
#define SCB_UFSR_INVSTATE               (uint16_t) (1 << 1)                                     /* Invalid state - PC stacked for exception return attempts illegal use of epsr */
#define SCB_UFSR_INVPC                  (uint16_t) (1 << 2)                                     /* Invalid PC load */
#define SCB_UFSR_NOCP                   (uint16_t) (1 << 3)                                     /* No coprocessor */
#define SCB_UFSR_UNALIGNED              (uint16_t) (1 << 8)                                     /* Unaligned access */
#define SCB_UFSR_DIVBYZERO              (uint16_t) (1 << 9)                                     /* Divide by zero */

#define SCB_CPACR_CP10_FULL             (uint32_t) (0x3 << 20)                                  /* Access privileges for coprocessor 10 (FPU) */
#define SCB_CPACR_CP11_FULL             (uint32_t) (0x3 << 22)                                  /* Access privileges for coprocessor 11 (FPU) */

/* Memory Protection Unit */
/* See pg. 183 in STM32F4 Prog Ref (PM0214) */
#define MPU_CTRL_ENABLE                 (uint32_t) (1 << 0)                                     /* Enables MPU */
#define MPU_CTRL_HFNMIENA               (uint32_t) (1 << 1)                                     /* Enables MPU during Hardfault, NMI, and Faultmask handlers */
#define MPU_CTRL_PRIVDEFENA             (uint32_t) (1 << 2)                                     /* Enable privileged software access to default memory map */

#define MPU_RASR_ENABLE                 (uint32_t) (1 << 0)                                     /* Enable region */
#define MPU_RASR_SIZE(x)                (uint32_t) (x << 1)                                     /* Region size (2^(x+1) bytes) */
#define MPU_RASR_SHARE_CACHE_WBACK      (uint32_t) (1 << 16) | (1 << 17) | (1 << 18)            /* Sharable, Cachable, Write-Back */
#define MPU_RASR_SHARE_NOCACHE_WBACK    (uint32_t) (1 << 16) | (0 << 17) | (1 << 18)            /* Sharable, Not Cachable, Write-Back */
#define MPU_RASR_AP_PRIV_NO_UN_NO       (uint32_t) (0 << 24)                                    /* No access for any */
#define MPU_RASR_AP_PRIV_RW_UN_NO       (uint32_t) (1 << 24)                                    /* No access for any */
#define MPU_RASR_AP_PRIV_RW_UN_RO       (uint32_t) (2 << 24)                                    /* Unprivileged Read Only Permissions */
#define MPU_RASR_AP_PRIV_RW_UN_RW       (uint32_t) (3 << 24)                                    /* All RW Permissions */
#define MPU_RASR_AP_PRIV_RO_UN_NO       (uint32_t) (5 << 24)                                    /* Privileged RO Permissions, Unpriv no access */
#define MPU_RASR_AP_PRIV_RO_UN_RO       (uint32_t) (6 << 24)                                    /* All RO Permissions */
#define MPU_RASR_XN                     (uint32_t) (1 << 28)                                    /* MPU Region Execute Never */

/* Debug */
#define DEBUG_DHCSR_DEBUGEN             (uint32_t) (1 << 0)                                     /* Debug enabled */

#define DEBUG_DEMCR_TRCENA              (uint32_t) (1 << 24)                                    /* DWT and ITM enable */

/* Floating Point Unit (FPU)
 * ST PM0214 (Cortex M4 Programming Manual) pg. 236 */
#define FPU_CCR_ASPEN                   (uint32_t) (1 << 31)                                    /* FPU Automatic State Preservation */

/* Trace Port Interface Unit (TPIU) */
#define TPIU_SPPR_PARALLEL              (uint32_t) (0x0)                                        /* TPIU Parallel trace mode */
#define TPIU_SPPR_SWO_MACHESTER         (uint32_t) (0x1)                                        /* TPIU SWO trace mode (Manchester encoding) */
#define TPIU_SPPR_SWO_NRZ               (uint32_t) (0x2)                                        /* TPIU SWO trace mode (NRZ encoding) */

#define TPIU_FFCR_FONFLIN               (uint32_t) (1 << 4)                                     /* TPIU Enable flush on FLUSHIN */
#define TPIU_FFCR_FONTRIG               (uint32_t) (1 << 5)                                     /* TPIU Enable flush on trigger */
#define TPIU_FFCR_FONMAN                (uint32_t) (1 << 6)                                     /* TPIU Manual flush */
#define TPIU_FFCR_TRIGIN                (uint32_t) (1 << 8)                                     /* TPIU Trigger on TRIGIN */
#define TPIU_FFCR_TRIGEVT               (uint32_t) (1 << 9)                                     /* TPIU Trigger on trigger event */
#define TPIU_FFCR_TRIGFL                (uint32_t) (1 << 10)                                    /* TPIU Trigger on flush complete */
#define TPIU_FFCR_STOPFL                (uint32_t) (1 << 12)                                    /* TPIU Stop formatter after flush */
#define TPIU_FFCR_STOPTRIG              (uint32_t) (1 << 13)                                    /* TPIU Stop formatter after trigger */
#define TPIU_FFCR_STOPMAN               (uint32_t) (1 << 14)                                    /* TPIU Stop formatter manually */

#endif
