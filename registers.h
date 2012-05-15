/* STM32F4 Registers and Memory Locations */

extern const uint32_t _skernel;
extern const uint32_t _ekernel;

/* Make a SVC call */
#define _svc(x)     asm volatile ("svc  %0  \n" :: "i" (x))

/* Memory Map */
#define FLASH_BASE          (uint32_t) (0x08000000)                     /* Flash Memory Base Address */
#define RAM_BASE            (uint32_t) (0x20000000)                     /* RAM Base Address */
#define ETHRAM_BASE         (uint32_t) (0x2001C000)                     /* ETHRAM Base Address */
#define CCMRAM_BASE         (uint32_t) (0x10000000)                     /* CCMRAM Base Address - Accessable only to CPU */

/* Peripheral Map */
#define PERIPH_BASE         (uint32_t) (0x40000000)                     /* Peripheral base address */
#define APB1PERIPH_BASE     (PERIPH_BASE)
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000)

#define GPIOD_BASE          (AHB1PERIPH_BASE + 0x0C00)                  /* GPIO Port D base address */
#define PWR_BASE            (APB1PERIPH_BASE + 0x7000)                  /* Power Control base address */
#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800)                  /* Reset and Clock Control base address */
#define FLASH_R_BASE        (AHB1PERIPH_BASE + 0x3C00)                  /* Flash registers base address */

/* System Control Map */
#define SCS_BASE            (uint32_t) (0xE000E000)                     /* System Control Space Base Address */
#define SCB_BASE            (SCS_BASE + 0x0D00)                         /* System Control Block Base Address */
#define MPU_BASE            (SCB_BASE + 0x0090)                         /* MPU Block Base Address */

/* GPIO Port D (GPIOD) */
#define GPIOD_MODER         (volatile uint32_t *) (GPIOD_BASE + 0x00)  /* Port D mode register */
#define LED_ODR             (volatile uint32_t *) (GPIOD_BASE + 0x14)  /* LED Output Data Register */

/* Power Control (PWR) */
#define PWR_CR              (volatile uint32_t *) (PWR_BASE + 0x00)    /* Power Control Register */
#define PWR_CSR             (volatile uint32_t *) (PWR_BASE + 0x04)    /* Power Control/Status Register */

/* Reset and Clock Control (RCC) */
#define RCC_CR              (volatile uint32_t *) (RCC_BASE + 0x00)    /* Clock Control Register */
#define RCC_PLLCFGR         (volatile uint32_t *) (RCC_BASE + 0x04)    /* PLL Configuration Register */
#define RCC_CFGR            (volatile uint32_t *) (RCC_BASE + 0x08)    /* Clock Configuration Register */
#define RCC_CIR             (volatile uint32_t *) (RCC_BASE + 0x0C)    /* Clock Interrupt Register */
#define RCC_AHB1ENR         (volatile uint32_t *) (RCC_BASE + 0x30)    /* AHB1 Enable Register */
#define RCC_APB1ENR         (volatile uint32_t *) (RCC_BASE + 0x40)    /* APB1 Peripheral Clock Enable Register */

/* Flash Registers (FLASH) */
#define FLASH_ACR           (volatile uint32_t *) (FLASH_R_BASE + 0x00)/* Flash Access Control Register */

/* System Control Block (SCB) */
#define SCB_VTOR            (volatile uint32_t *) (SCB_BASE + 0x008)   /* Vector Table Offset Register */
#define SCB_CPACR           (volatile uint32_t *) (SCB_BASE + 0x088)   /* Coprocessor (FPU) Access Control Register */
#define SCB_SHCSR           (volatile uint32_t *) (SCB_BASE + 0x024)   /* System Handler Control and State Register */

/* Memory Protection Unit (MPU) 
 * ST PM0214 (Cortex M$ Programming Manual) pg. 195 */
#define MPU_TYPER           (volatile uint32_t *) (MPU_BASE + 0x00)    /* MPU Type Register - Describes HW MPU */
#define MPU_CTRL            (volatile uint32_t *) (MPU_BASE + 0x04)    /* MPU Control Register */
#define MPU_RNR             (volatile uint32_t *) (MPU_BASE + 0x08)    /* MPU Region Number Register */
#define MPU_RBAR            (volatile uint32_t *) (MPU_BASE + 0x0C)    /* MPU Region Base Address Register */
#define MPU_RASR            (volatile uint32_t *) (MPU_BASE + 0x10)    /* MPU Region Attribute and Size Register */



/* Bit Masks - See RM0090 Reference Manual for STM32F4 for details */
#define  PWR_CR_VOS             (uint16_t) (0x4000)         /* Regulator voltage scaling output selection */

#define  RCC_CR_HSEON           (uint32_t) (0x00010000)     /* Enable HSE */
#define  RCC_CR_HSERDY          (uint32_t) (0x00020000)     /* HSE Ready */
#define  RCC_CR_PLLON           (uint32_t) (0x01000000)     /* Main PLL Enable */
#define  RCC_CR_PLLRDY          (uint32_t) (0x02000000)     /* Main PLL clock ready */

#define  RCC_CFGR_SW            (uint32_t) (0x00000003)     /* SW[1:0] bits (System clock Switch) */
#define  RCC_CFGR_SW_PLL        (uint32_t) (0x00000002)     /* PLL selected as system clock */
#define  RCC_CFGR_SWS           (uint32_t) (0x0000000C)     /* SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_CFGR_SWS_PLL       (uint32_t) (0x00000008)     /* PLL used as system clock */
#define  RCC_CFGR_HPRE_DIV1     (uint32_t) (0x00000000)     /* SYSCLK not divided (highest frequency) */
#define  RCC_CFGR_PPRE2_DIV2    (uint32_t) (0x00008000)     /* HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4    (uint32_t) (0x00001400)     /* HCLK divided by 4 */

#define  RCC_PLLCFGR_PLLSRC_HSE (uint32_t) (0x00400000)     /* HSE oscillator selected as clock entry */

#define  RCC_APB1ENR_PWREN      (uint32_t) (0x10000000)     /* Power Interface Clock Enable */

#define  FLASH_ACR_ICEN         (uint32_t) (0x00000200)     /* Instruction Cache Enable */
#define  FLASH_ACR_DCEN         (uint32_t) (0x00000400)     /* Data Cache Enable */
#define  FLASH_ACR_LATENCY_5WS  (uint32_t) (0x00000005)     /* 5 Wait States Latency */
