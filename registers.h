/* STM32F4 Registers and Memory Locations */

extern const uint32_t _skernel;
extern const uint32_t _ekernel;

/* Make a SVC call */
#define _svc(x)                         asm volatile ("svc  %0  \n" :: "i" (x))

/* Memory Map */
#define MEMORY_BASE                     (uint32_t) (0x00000000)                     /* Base of memory map */
#define FLASH_BASE                      (uint32_t) (0x08000000)                     /* Flash Memory Base Address */
#define RAM_BASE                        (uint32_t) (0x20000000)                     /* RAM Base Address */
#define ETHRAM_BASE                     (uint32_t) (0x2001C000)                     /* ETHRAM Base Address */
#define CCMRAM_BASE                     (uint32_t) (0x10000000)                     /* CCMRAM Base Address - Accessable only to CPU */

/* Peripheral Map */
#define PERIPH_BASE                     (uint32_t) (0x40000000)                     /* Peripheral base address */
#define PRIV_PERIPH_BASE                (uint32_t) (0xED000000)                     /* Private peripheral base address */
#define APB1PERIPH_BASE                 (PERIPH_BASE)
#define APB2PERIPH_BASE                 (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE                 (PERIPH_BASE + 0x00020000)

#define PWR_BASE                        (APB1PERIPH_BASE + 0x7000)                  /* Power Control base address */
#define USART1_BASE                     (APB2PERIPH_BASE + 0x1000)                  /* USART1 Base Address */
#define GPIOB_BASE                      (AHB1PERIPH_BASE + 0x0400)                  /* GPIO Port B base address */
#define GPIOD_BASE                      (AHB1PERIPH_BASE + 0x0C00)                  /* GPIO Port D base address */
#define RCC_BASE                        (AHB1PERIPH_BASE + 0x3800)                  /* Reset and Clock Control base address */
#define FLASH_R_BASE                    (AHB1PERIPH_BASE + 0x3C00)                  /* Flash registers base address */

/* System Control Map */
#define SCS_BASE                        (uint32_t) (0xE000E000)                     /* System Control Space Base Address */
#define SYSTICK_BASE                    (SCS_BASE + 0x0010)                         /* Systick Registers Base Address */
#define NVIC_BASE                       (SCS_BASE + 0x0100)                         /* Nested Vector Interrupt Control */
#define SCB_BASE                        (SCS_BASE + 0x0D00)                         /* System Control Block Base Address */
#define MPU_BASE                        (SCB_BASE + 0x0090)                         /* MPU Block Base Address */
#define FPU_BASE                        (SCB_BASE + 0x0230)                         /* FPU Block Base Address */

/* GPIO Port B (GPIOB) */
#define GPIOB_MODER                     (volatile uint32_t *) (GPIOB_BASE + 0x00)   /* Port B mode register */
#define GPIOB_OTYPER                    (volatile uint32_t *) (GPIOB_BASE + 0x04)   /* Port B output type register */
#define GPIOB_OSPEEDR                   (volatile uint32_t *) (GPIOB_BASE + 0x08)   /* Port B output speed register */
#define GPIOB_PUPDR                     (volatile uint32_t *) (GPIOB_BASE + 0x0C)   /* Port B pull up/down register */
#define GPIOB_AFRL                      (volatile uint32_t *) (GPIOB_BASE + 0x20)   /* Port B alternate function low register */
#define GPIOB_AFRH                      (volatile uint32_t *) (GPIOB_BASE + 0x24)   /* Port B alternate function high register */

/* GPIO Port D (GPIOD) */
#define GPIOD_MODER                     (volatile uint32_t *) (GPIOD_BASE + 0x00)   /* Port D mode register */
#define LED_ODR                         (volatile uint32_t *) (GPIOD_BASE + 0x14)   /* LED Output Data Register */

/* USART 1 */
#define USART1_SR                       (volatile uint32_t *) (USART1_BASE + 0x00)  /* USART1 status register */
#define USART1_DR                       (volatile uint32_t *) (USART1_BASE + 0x04)  /* USART1 data register */
#define USART1_BRR                      (volatile uint32_t *) (USART1_BASE + 0x08)  /* USART1 baud rate register */
#define USART1_CR1                      (volatile uint32_t *) (USART1_BASE + 0x0C)  /* USART1 control register 1 */
#define USART1_CR2                      (volatile uint32_t *) (USART1_BASE + 0x10)  /* USART1 control register 2 */
#define USART1_CR3                      (volatile uint32_t *) (USART1_BASE + 0x14)  /* USART1 control register 3 */
#define USART1_GTPR                     (volatile uint32_t *) (USART1_BASE + 0x18)  /* USART1 gaurd time and prescale register */

/* SPI2 */
#define SPI2_APB1EN 0x00004000
#define SPI2        0x40003800
#define SPI2_CR1     *(volatile uint32_t *) (SPI2 + 0x00)
#define SPI2_CR2     *(volatile uint32_t *) (SPI2 + 0x04)
#define SPI2_SR      *(volatile uint32_t *) (SPI2 + 0x08)
#define SPI2_DR      *(volatile uint32_t *) (SPI2 + 0x0c)
#define SPI2_CRCPR   *(volatile uint32_t *) (SPI2 + 0x10)
#define SPI2_RXCRCR  *(volatile uint32_t *) (SPI2 + 0x14)
#define SPI2_TXCRCR  *(volatile uint32_t *) (SPI2 + 0x18)
#define SPI2_I2SCFGR *(volatile uint32_t *) (SPI2 + 0x1c)
#define SPI2_I2SPR   *(volatile uint32_t *) (SPI2 + 0x20)
#define GPIOD       0x40020c00  /* Port D base address */
#define GPIOD_MODER *(volatile uint32_t *) (GPIOD + 0x00)   /* Port D mode register */
#define LED_ODR     *(volatile uint32_t *) (GPIOD + 0x14)   /* LED Output Data Register */

/* Power Control (PWR) */
#define PWR_CR                          (volatile uint32_t *) (PWR_BASE + 0x00)     /* Power Control Register */
#define PWR_CSR                         (volatile uint32_t *) (PWR_BASE + 0x04)     /* Power Control/Status Register */

/* Reset and Clock Control (RCC) */
#define RCC_CR                          (volatile uint32_t *) (RCC_BASE + 0x00)     /* Clock Control Register */
#define RCC_PLLCFGR                     (volatile uint32_t *) (RCC_BASE + 0x04)     /* PLL Configuration Register */
#define RCC_CFGR                        (volatile uint32_t *) (RCC_BASE + 0x08)     /* Clock Configuration Register */
#define RCC_CIR                         (volatile uint32_t *) (RCC_BASE + 0x0C)     /* Clock Interrupt Register */
#define RCC_AHB1ENR                     (volatile uint32_t *) (RCC_BASE + 0x30)     /* AHB1 Enable Register */
#define RCC_APB1ENR                     (volatile uint32_t *) (RCC_BASE + 0x40)     /* APB1 Peripheral Clock Enable Register */
#define RCC_APB2ENR                     (volatile uint32_t *) (RCC_BASE + 0x44)     /* APB2 Peripheral Clock Enable Register */

/* Flash Registers (FLASH) */
#define FLASH_ACR                       (volatile uint32_t *) (FLASH_R_BASE + 0x00) /* Flash Access Control Register */

/* SysTick Timer */
#define SYSTICK_CTL                     (volatile uint32_t *) (SYSTICK_BASE)        /* Control register for SysTick timer peripheral */
#define SYSTICK_RELOAD                  (volatile uint32_t *) (SYSTICK_BASE + 0x04) /* Value assumed by timer upon reload */
#define SYSTICK_VAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x08) /* Current value of timer */
#define SYSTICK_CAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x0C) /* Calibration settings/value register */

/* Nested Vector Interrupt Controller */
#define NVIC_ISER0                      (volatile uint32_t *) (NVIC_BASE + 0x000)   /* Interrupt set-enable register 0 */
#define NVIC_ISER1                      (volatile uint32_t *) (NVIC_BASE + 0x004)   /* Interrupt set-enable register 1 */
#define NVIC_ISER2                      (volatile uint32_t *) (NVIC_BASE + 0x008)   /* Interrupt set-enable register 2 */
#define NVIC_ICER0                      (volatile uint32_t *) (NVIC_BASE + 0x080)   /* Interrupt clear-enable register 0 */
#define NVIC_ICER1                      (volatile uint32_t *) (NVIC_BASE + 0x084)   /* Interrupt clear-enable register 1 */
#define NVIC_ICER2                      (volatile uint32_t *) (NVIC_BASE + 0x088)   /* Interrupt clear-enable register 2 */
#define NVIC_ICPR0                      (volatile uint32_t *) (NVIC_BASE + 0x180)   /* Interrupt clear-pending register 0 */
#define NVIC_ICPR1                      (volatile uint32_t *) (NVIC_BASE + 0x184)   /* Interrupt clear-pending register 1 */
#define NVIC_ICPR2                      (volatile uint32_t *) (NVIC_BASE + 0x188)   /* Interrupt clear-pending register 2 */

/* System Control Block (SCB) */
#define SCB_ICSR                        (volatile uint32_t *) (SCB_BASE + 0x004)    /* Interrupt Control and State Register */
#define SCB_VTOR                        (volatile uint32_t *) (SCB_BASE + 0x008)    /* Vector Table Offset Register */
#define SCB_CPACR                       (volatile uint32_t *) (SCB_BASE + 0x088)    /* Coprocessor (FPU) Access Control Register */
#define SCB_SHCSR                       (volatile uint32_t *) (SCB_BASE + 0x024)    /* System Handler Control and State Register */
#define SCB_CFSR                        (volatile uint32_t *) (SCB_BASE + 0x028)    /* Configurable fault status register - Describes Usage, Bus, and Memory faults */
#define SCB_HFSR                        (volatile uint32_t *) (SCB_BASE + 0x02C)    /* Hard fault status register - Describes hard fault */
#define SCB_MMFAR                       (volatile uint32_t *) (SCB_BASE + 0x034)    /* Memory management fault address register - Address that caused fault */
#define SCB_BFAR                        (volatile uint32_t *) (SCB_BASE + 0x038)    /* Bus fault address register - Address that caused fault */

/* Memory Protection Unit (MPU) 
 * ST PM0214 (Cortex M4 Programming Manual) pg. 195 */
#define MPU_TYPER                       (volatile uint32_t *) (MPU_BASE + 0x00)     /* MPU Type Register - Describes HW MPU */
#define MPU_CTRL                        (volatile uint32_t *) (MPU_BASE + 0x04)     /* MPU Control Register */
#define MPU_RNR                         (volatile uint32_t *) (MPU_BASE + 0x08)     /* MPU Region Number Register */
#define MPU_RBAR                        (volatile uint32_t *) (MPU_BASE + 0x0C)     /* MPU Region Base Address Register */
#define MPU_RASR                        (volatile uint32_t *) (MPU_BASE + 0x10)     /* MPU Region Attribute and Size Register */

/* Floating Point Unit (FPU)            
 * ST PM0214 (Cortex M4 Prog            ramming Manual) pg. 236 */
#define FPU_CCR                         (volatile uint32_t *) (FPU_BASE + 0x04)     /* FPU Context Control Register */
#define FPU_CAR                         (volatile uint32_t *) (FPU_BASE + 0x08)     /* FPU Context Address Register */


/* Bit Masks - See RM0090 Reference Manual for STM32F4 for details */
#define PWR_CR_VOS                      (uint16_t) (0x4000)                         /* Regulator voltage scaling output selection */

#define RCC_CR_HSEON                    (uint32_t) (0x00010000)                     /* Enable HSE */
#define RCC_CR_HSERDY                   (uint32_t) (0x00020000)                     /* HSE Ready */
#define RCC_CR_PLLON                    (uint32_t) (0x01000000)                     /* Main PLL Enable */
#define RCC_CR_PLLRDY                   (uint32_t) (0x02000000)                     /* Main PLL clock ready */

#define RCC_CFGR_SW                     (uint32_t) (0x00000003)                     /* SW[1:0] bits (System clock Switch) */
#define RCC_CFGR_SW_PLL                 (uint32_t) (0x00000002)                     /* PLL selected as system clock */
#define RCC_CFGR_SWS                    (uint32_t) (0x0000000C)                     /* SWS[1:0] bits (System Clock Switch Status) */
#define RCC_CFGR_SWS_PLL                (uint32_t) (0x00000008)                     /* PLL used as system clock */
#define RCC_CFGR_HPRE_DIV1              (uint32_t) (0x00000000)                     /* SYSCLK not divided (highest frequency) */
#define RCC_CFGR_PPRE2_DIV2             (uint32_t) (0x00008000)                     /* HCLK divided by 2 */
#define RCC_CFGR_PPRE1_DIV4             (uint32_t) (0x00001400)                     /* HCLK divided by 4 */

#define RCC_PLLCFGR_PLLSRC_HSE          (uint32_t) (0x00400000)                     /* HSE oscillator selected as clock entry */

#define RCC_APB1ENR_PWREN               (uint32_t) (0x10000000)                     /* Power Interface Clock Enable */

#define FLASH_ACR_ICEN                  (uint32_t) (0x00000200)                     /* Instruction Cache Enable */
#define FLASH_ACR_DCEN                  (uint32_t) (0x00000400)                     /* Data Cache Enable */
#define FLASH_ACR_LATENCY_5WS           (uint32_t) (0x00000005)                     /* 5 Wait States Latency */

/* GPIO */
#define GPIO_MODER_ALT                  (uint32_t) (0x2)                            /* Sets GPIO pin to alternative function mode */
#define GPIO_AF_USART13                 (uint32_t) (0x7)                            /* GPIO USART1-3 mode */

/* USART */
#define USART_SR_TC                     (uint32_t) (1 << 6)                         /* USART Transmission Complete */
#define USART_SR_RXNE                   (uint32_t) (1 << 5)                         /* USART Read data register not empty */
#define USART_CR1_UE                    (uint32_t) (1 << 13)                        /* USART Enable */
#define USART_CR1_RXNEIE                (uint32_t) (1 << 5)                         /* RXNE Interrupt Enablr */
#define USART_CR1_TE                    (uint32_t) (1 << 3)                         /* USART Transmit Enable */
#define USART_CR1_RE                    (uint32_t) (1 << 2)                         /* USART Receive Enable */

/* System Control Block */
#define SCB_SHCSR_MEMFAULTENA           (uint32_t) (1 << 16)                        /* Enables Memory Management Fault */
#define SCB_SHCSR_BUSFAULTENA           (uint32_t) (1 << 17)                        /* Enables Bus Fault */
#define SCB_SHCSR_USEFAULTENA           (uint32_t) (1 << 18)                        /* Enables Usage Fault */

/* Hard Fault Status Register */        
#define SCB_HFSR_VECTTBL                (uint32_t) (1 << 1)                         /* Vector table hard fault.  Bus fault on vector table read during exception handling. */
#define SCB_HFSR_FORCED                 (uint32_t) (1 << 30)                        /* Forced hard fault.  Escalation of another fault. */

/* Memory Management Fault Status Register */
#define SCB_MMFSR_IACCVIOL              (uint8_t)  (1 << 0)                         /* Instruction access violation.  No address in MMFAR */
#define SCB_MMFSR_DACCVIOL              (uint8_t)  (1 << 1)                         /* Data access violation.  Address in MMFAR */
#define SCB_MMFSR_MUNSTKERR             (uint8_t)  (1 << 3)                         /* Fault on unstacking from exception.  No address in MMAR */
#define SCB_MMFSR_MSTKERR               (uint8_t)  (1 << 4)                         /* Fault on stacking for exception.  No address in MMFAR */
#define SCB_MMFSR_MLSPERR               (uint8_t)  (1 << 5)                         /* Fault during FP lazy state preservation. */
#define SCB_MMFSR_MMARVALID             (uint8_t)  (1 << 7)                         /* MMFAR holds valid address */

/* Bus Fault Status Register */
#define SCB_BFSR_IBUSERR                (uint8_t)  (1 << 0)                         /* Instruction bus error.  No address in BFAR */
#define SCB_BFSR_PRECISERR              (uint8_t)  (1 << 1)                         /* Precise data bus error.  Address in BFAR */
#define SCB_BFSR_IMPRECISERR            (uint8_t)  (1 << 2)                         /* Imprecise data bus error.  No address in BFAR */
#define SCB_BFSR_UNSTKERR               (uint8_t)  (1 << 3)                         /* Fault on unstacking from exception.  No address in BFAR */
#define SCB_BFSR_STKERR                 (uint8_t)  (1 << 4)                         /* Fault on stacking for exception.  No address in BFAR */
#define SCB_BFSR_LSPERR                 (uint8_t)  (1 << 5)                         /* Fault on FP lazy state preservation. */
#define SCB_BFSR_BFARVALID              (uint8_t)  (1 << 7)                         /* BFAR holds valid address */

/* Usage Fault Status Register */
#define SCB_UFSR_UNDEFINSTR             (uint16_t) (1 << 0)                         /* Undefined instruction */
#define SCB_UFSR_INVSTATE               (uint16_t) (1 << 1)                         /* Invalid state - PC stacked for exception return attempts illegal use of epsr */
#define SCB_UFSR_INVPC                  (uint16_t) (1 << 2)                         /* Invalid PC load */
#define SCB_UFSR_NOCP                   (uint16_t) (1 << 3)                         /* No coprocessor */
#define SCB_UFSR_UNALIGNED              (uint16_t) (1 << 8)                         /* Unaligned access */
#define SCB_UFSR_DIVBYZERO              (uint16_t) (1 << 9)                         /* Divide by zero */

/* Memory Protection Unit */
/* See pg. 183 in STM32F4 Prog Ref (PM0214) */
#define MPU_CTRL_ENABLE                 (uint32_t) (1 << 0)                         /* Enables MPU */
#define MPU_CTRL_HFNMIENA               (uint32_t) (1 << 1)                         /* Enables MPU during Hardfault, NMI, and Faultmask handlers */
#define MPU_CTRL_PRIVDEFENA             (uint32_t) (1 << 2)                         /* Enable privileged software access to default memory map */

#define MPU_RASR_ENABLE                 (uint32_t) (1 << 0)                         /* Enable region */
#define MPU_RASR_SIZE(x)                (uint32_t) (x << 1)                         /* Region size (2^(x+1) bytes) */
#define MPU_RASR_SHARE_CACHE_WBACK      (uint32_t) (1 << 16) | (1 << 17) | (1 << 18)/* Sharable, Cachable, Write-Back */
#define MPU_RASR_SHARE_NOCACHE_WBACK    (uint32_t) (1 << 16) | (0 << 17) | (1 << 18)/* Sharable, Not Cachable, Write-Back */
#define MPU_RASR_AP_PRIV_NO_UN_NO       (uint32_t) (0 << 24)                        /* No access for any */
#define MPU_RASR_AP_PRIV_RW_UN_NO       (uint32_t) (1 << 24)                        /* No access for any */
#define MPU_RASR_AP_PRIV_RW_UN_RO       (uint32_t) (2 << 24)                        /* Unprivileged Read Only Permissions */
#define MPU_RASR_AP_PRIV_RW_UN_RW       (uint32_t) (3 << 24)                        /* All RW Permissions */
#define MPU_RASR_AP_PRIV_RO_UN_NO       (uint32_t) (5 << 24)                        /* Privileged RO Permissions, Unpriv no access */
#define MPU_RASR_AP_PRIV_RO_UN_RO       (uint32_t) (6 << 24)                        /* All RO Permissions */
#define MPU_RASR_XN                     (uint32_t) (1 << 28)                        /* MPU Region Execute Never */

/* Floating Point Unit (FPU)
 * ST PM0214 (Cortex M4 Programming Manual) pg. 236 */
#define FPU_CCR_ASPEN                   (uint32_t) (1 << 31)                        /* FPU Automatic State Preservation */
