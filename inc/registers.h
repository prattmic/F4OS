/* STM32F4 Registers and Memory Locations */

extern const uint32_t _skernel;
extern const uint32_t _ekernel;

inline uint8_t FAULTMASK(void) __attribute__((always_inline));
inline uint8_t FAULTMASK(void) {
    uint8_t val;

    asm("mrs    %[val], faultmask"
        :[val] "=r" (val)
        ::);

    return val;
}

/* Make a SVC call */
#define _svc(x)                         asm volatile ("svc  %0  \n" :: "i" (x))

/* Memory Map */
#define MEMORY_BASE                     (uint32_t) (0x00000000)                                 /* Base of memory map */
#define FLASH_BASE                      (uint32_t) (0x08000000)                                 /* Flash Memory Base Address */
#define RAM_BASE                        (uint32_t) (0x20000000)                                 /* RAM Base Address */
#define ETHRAM_BASE                     (uint32_t) (0x2001C000)                                 /* ETHRAM Base Address */
#define CCMRAM_BASE                     (uint32_t) (0x10000000)                                 /* CCMRAM Base Address - Accessible only to CPU */

/* Peripheral Map */
#define PERIPH_BASE                     (uint32_t) (0x40000000)                                 /* Peripheral base address */
#define PRIV_PERIPH_BASE                (uint32_t) (0xED000000)                                 /* Private peripheral base address */
#define APB1PERIPH_BASE                 (PERIPH_BASE)
#define APB2PERIPH_BASE                 (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE                 (PERIPH_BASE + 0x00020000)

#define TIM2_BASE                       (APB1PERIPH_BASE + 0x0000)                              /* Timer 2 base address */
#define PWR_BASE                        (APB1PERIPH_BASE + 0x7000)                              /* Power Control base address */
#define SPI2_BASE                       (APB1PERIPH_BASE + 0x3800)                              /* SPI2 base address */
#define SPI1_BASE                       (APB2PERIPH_BASE + 0x3000)                              /* SPI1 base address */
#define USART1_BASE                     (APB2PERIPH_BASE + 0x1000)                              /* USART1 Base Address */
#define GPIOA_BASE                      (AHB1PERIPH_BASE + 0x0000)                              /* GPIO Port A base address */
#define GPIOB_BASE                      (AHB1PERIPH_BASE + 0x0400)                              /* GPIO Port B base address */
#define GPIOD_BASE                      (AHB1PERIPH_BASE + 0x0C00)                              /* GPIO Port D base address */
#define GPIOE_BASE                      (AHB1PERIPH_BASE + 0x1000)                              /* GPIO Port E base address */
#define RCC_BASE                        (AHB1PERIPH_BASE + 0x3800)                              /* Reset and Clock Control base address */
#define FLASH_R_BASE                    (AHB1PERIPH_BASE + 0x3C00)                              /* Flash registers base address */
#define DMA1_BASE                       (AHB1PERIPH_BASE + 0x6000)                              /* DMA1 base address */
#define DMA2_BASE                       (AHB1PERIPH_BASE + 0x6400)                              /* DMA2 base address */

/* System Control Map */
#define SCS_BASE                        (uint32_t) (0xE000E000)                                 /* System Control Space Base Address */
#define SYSTICK_BASE                    (SCS_BASE + 0x0010)                                     /* Systick Registers Base Address */
#define NVIC_BASE                       (SCS_BASE + 0x0100)                                     /* Nested Vector Interrupt Control */
#define SCB_BASE                        (SCS_BASE + 0x0D00)                                     /* System Control Block Base Address */
#define MPU_BASE                        (SCB_BASE + 0x0090)                                     /* MPU Block Base Address */
#define FPU_BASE                        (SCB_BASE + 0x0230)                                     /* FPU Block Base Address */

/* Timer 2 (TIM2) */
#define TIM2_CR1                        (volatile uint32_t *) (TIM2_BASE + 0x00)                /* TIM2 control register 1 */
#define TIM2_CR2                        (volatile uint32_t *) (TIM2_BASE + 0x04)                /* TIM2 control register 2 */
#define TIM2_SMCR                       (volatile uint32_t *) (TIM2_BASE + 0x08)                /* TIM2 slave mode control register */
#define TIM2_DIER                       (volatile uint32_t *) (TIM2_BASE + 0x0C)                /* TIM2 DMA/Interrupt enable register */
#define TIM2_SR                         (volatile uint32_t *) (TIM2_BASE + 0x10)                /* TIM2 status register */
#define TIM2_EGR                        (volatile uint32_t *) (TIM2_BASE + 0x14)                /* TIM2 event generation register */
#define TIM2_CCMR1                      (volatile uint32_t *) (TIM2_BASE + 0x18)                /* TIM2 capture/compare mode register 1 */
#define TIM2_CCMR2                      (volatile uint32_t *) (TIM2_BASE + 0x1C)                /* TIM2 capture/compare mode register 2 */
#define TIM2_CCER                       (volatile uint32_t *) (TIM2_BASE + 0x20)                /* TIM2 capture/compare enable register */
#define TIM2_CNT                        (volatile uint32_t *) (TIM2_BASE + 0x24)                /* TIM2 counter */
#define TIM2_PSC                        (volatile uint32_t *) (TIM2_BASE + 0x28)                /* TIM2 prescaler */
#define TIM2_ARR                        (volatile uint32_t *) (TIM2_BASE + 0x2C)                /* TIM2 auto-reload register */
#define TIM2_CCR1                       (volatile uint32_t *) (TIM2_BASE + 0x34)                /* TIM2 capture/compare register 1 */
#define TIM2_CCR2                       (volatile uint32_t *) (TIM2_BASE + 0x38)                /* TIM2 capture/compare register 2 */
#define TIM2_CCR3                       (volatile uint32_t *) (TIM2_BASE + 0x3C)                /* TIM2 capture/compare register 3 */
#define TIM2_CCR4                       (volatile uint32_t *) (TIM2_BASE + 0x40)                /* TIM2 capture/compare register 4 */
#define TIM2_DCR                        (volatile uint32_t *) (TIM2_BASE + 0x48)                /* TIM2 DMA control register */
#define TIM2_DMAR                       (volatile uint32_t *) (TIM2_BASE + 0x4C)                /* TIM2 DMA address for full transfer */
#define TIM2_OR                         (volatile uint32_t *) (TIM2_BASE + 0x50)                /* TIM2 option register */

/* Power Control (PWR) */
#define PWR_CR                          (volatile uint32_t *) (PWR_BASE + 0x00)                 /* Power Control Register */
#define PWR_CSR                         (volatile uint32_t *) (PWR_BASE + 0x04)                 /* Power Control/Status Register */

/* SPI1 */
#define SPI1_CR1                        (volatile uint32_t *) (SPI1_BASE + 0x00)                /* SPI1 control register 1 */
#define SPI1_CR2                        (volatile uint32_t *) (SPI1_BASE + 0x04)                /* SPI1 control register 2 */
#define SPI1_SR                         (volatile uint32_t *) (SPI1_BASE + 0x08)                /* SPI1 status register */
#define SPI1_DR                         (volatile uint32_t *) (SPI1_BASE + 0x0c)                /* SPI1 data register */
#define SPI1_CRCPR                      (volatile uint32_t *) (SPI1_BASE + 0x10)                /* SPI1 CRC polynomial register */
#define SPI1_RXCRCR                     (volatile uint32_t *) (SPI1_BASE + 0x14)                /* SPI1 RX CRC register */
#define SPI1_TXCRCR                     (volatile uint32_t *) (SPI1_BASE + 0x18)                /* SPI1 TX CRC register */
#define SPI1_I2SCFGR                    (volatile uint32_t *) (SPI1_BASE + 0x1c)                /* SPI1 I2C configuration register */
#define SPI1_I2SPR                      (volatile uint32_t *) (SPI1_BASE + 0x20)                /* SPI1 I2C prescaler register */

/* SPI2 */
#define SPI2_CR1                        (volatile uint32_t *) (SPI2_BASE + 0x00)                /* SPI2 control register 1 */
#define SPI2_CR2                        (volatile uint32_t *) (SPI2_BASE + 0x04)                /* SPI2 control register 2 */
#define SPI2_SR                         (volatile uint32_t *) (SPI2_BASE + 0x08)                /* SPI2 status register */
#define SPI2_DR                         (volatile uint32_t *) (SPI2_BASE + 0x0c)                /* SPI2 data register */
#define SPI2_CRCPR                      (volatile uint32_t *) (SPI2_BASE + 0x10)                /* SPI2 CRC polynomial register */
#define SPI2_RXCRCR                     (volatile uint32_t *) (SPI2_BASE + 0x14)                /* SPI2 RX CRC register */
#define SPI2_TXCRCR                     (volatile uint32_t *) (SPI2_BASE + 0x18)                /* SPI2 TX CRC register */
#define SPI2_I2SCFGR                    (volatile uint32_t *) (SPI2_BASE + 0x1c)                /* SPI2 I2C configuration register */
#define SPI2_I2SPR                      (volatile uint32_t *) (SPI2_BASE + 0x20)                /* SPI2 I2C prescaler register */

/* USART 1 */
#define USART1_SR                       (volatile uint32_t *) (USART1_BASE + 0x00)              /* USART1 status register */
#define USART1_DR                       (volatile uint32_t *) (USART1_BASE + 0x04)              /* USART1 data register */
#define USART1_BRR                      (volatile uint32_t *) (USART1_BASE + 0x08)              /* USART1 baud rate register */
#define USART1_CR1                      (volatile uint32_t *) (USART1_BASE + 0x0C)              /* USART1 control register 1 */
#define USART1_CR2                      (volatile uint32_t *) (USART1_BASE + 0x10)              /* USART1 control register 2 */
#define USART1_CR3                      (volatile uint32_t *) (USART1_BASE + 0x14)              /* USART1 control register 3 */
#define USART1_GTPR                     (volatile uint32_t *) (USART1_BASE + 0x18)              /* USART1 gaurd time and prescale register */

/* GPIO Port A (GPIOA) */
#define GPIOA_MODER                     (volatile uint32_t *) (GPIOA_BASE + 0x00)               /* Port A mode register */
#define GPIOA_OTYPER                    (volatile uint32_t *) (GPIOA_BASE + 0x04)               /* Port A output type register */
#define GPIOA_OSPEEDR                   (volatile uint32_t *) (GPIOA_BASE + 0x08)               /* Port A output speed register */
#define GPIOA_PUPDR                     (volatile uint32_t *) (GPIOA_BASE + 0x0C)               /* Port A pull up/down register */
#define GPIOA_AFRL                      (volatile uint32_t *) (GPIOA_BASE + 0x20)               /* Port A alternate function low register */
#define GPIOA_AFRH                      (volatile uint32_t *) (GPIOA_BASE + 0x24)               /* Port A alternate function high register */

/* GPIO Port B (GPIOB) */
#define GPIOB_MODER                     (volatile uint32_t *) (GPIOB_BASE + 0x00)               /* Port B mode register */
#define GPIOB_OTYPER                    (volatile uint32_t *) (GPIOB_BASE + 0x04)               /* Port B output type register */
#define GPIOB_OSPEEDR                   (volatile uint32_t *) (GPIOB_BASE + 0x08)               /* Port B output speed register */
#define GPIOB_PUPDR                     (volatile uint32_t *) (GPIOB_BASE + 0x0C)               /* Port B pull up/down register */
#define GPIOB_AFRL                      (volatile uint32_t *) (GPIOB_BASE + 0x20)               /* Port B alternate function low register */
#define GPIOB_AFRH                      (volatile uint32_t *) (GPIOB_BASE + 0x24)               /* Port B alternate function high register */

/* GPIO Port E (GPIOE) */
#define GPIOE_MODER                     (volatile uint32_t *) (GPIOE_BASE + 0x00)               /* Port E mode register */
#define GPIOE_OTYPER                    (volatile uint32_t *) (GPIOE_BASE + 0x04)               /* Port E output type register */
#define GPIOE_OSPEEDR                   (volatile uint32_t *) (GPIOE_BASE + 0x08)               /* Port E output speed register */
#define GPIOE_PUPDR                     (volatile uint32_t *) (GPIOE_BASE + 0x0C)               /* Port E pull up/down register */
#define GPIOE_ODR                       (volatile uint32_t *) (GPIOE_BASE + 0x14)               /* Port E output data register */
#define GPIOE_AFRL                      (volatile uint32_t *) (GPIOE_BASE + 0x20)               /* Port E alternate function low register */
#define GPIOE_AFRH                      (volatile uint32_t *) (GPIOE_BASE + 0x24)               /* Port E alternate function high register */

/* GPIO Port D (GPIOD) */
#define GPIOD_MODER                     (volatile uint32_t *) (GPIOD_BASE + 0x00)               /* Port D mode register */
#define LED_ODR                         (volatile uint32_t *) (GPIOD_BASE + 0x14)               /* LED Output Data Register */

/* Reset and Clock Control (RCC) */
#define RCC_CR                          (volatile uint32_t *) (RCC_BASE + 0x00)                 /* Clock Control Register */
#define RCC_PLLCFGR                     (volatile uint32_t *) (RCC_BASE + 0x04)                 /* PLL Configuration Register */
#define RCC_CFGR                        (volatile uint32_t *) (RCC_BASE + 0x08)                 /* Clock Configuration Register */
#define RCC_CIR                         (volatile uint32_t *) (RCC_BASE + 0x0C)                 /* Clock Interrupt Register */
#define RCC_AHB1RSTR                    (volatile uint32_t *) (RCC_BASE + 0x10)                 /* AHB1 reset Register */
#define RCC_AHB1ENR                     (volatile uint32_t *) (RCC_BASE + 0x30)                 /* AHB1 Enable Register */
#define RCC_APB1ENR                     (volatile uint32_t *) (RCC_BASE + 0x40)                 /* APB1 Peripheral Clock Enable Register */
#define RCC_APB2ENR                     (volatile uint32_t *) (RCC_BASE + 0x44)                 /* APB2 Peripheral Clock Enable Register */

/* Flash Registers (FLASH) */
#define FLASH_ACR                       (volatile uint32_t *) (FLASH_R_BASE + 0x00)             /* Flash Access Control Register */

/* Direct Memory Access 1 (DMA) */
#define DMA1_LISR                       (volatile uint32_t *) (DMA1_BASE + 0x00)                /* DMA1 low interrupt status register */
#define DMA1_HISR                       (volatile uint32_t *) (DMA1_BASE + 0x04)                /* DMA1 high interrupt status register */
#define DMA1_LIFCR                      (volatile uint32_t *) (DMA1_BASE + 0x08)                /* DMA1 low interrupt flag clear register */
#define DMA1_HIFCR                      (volatile uint32_t *) (DMA1_BASE + 0x0C)                /* DMA1 high interrupt flag clear register */
/* Stream 0 */
#define DMA1_S0CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*0))     /* DMA1 stream 0 configuration register */
#define DMA1_S0NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*0))     /* DMA1 stream 0 number of data register */
#define DMA1_S0PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*0))     /* DMA1 stream 0 peripheral address register */
#define DMA1_S0M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*0))     /* DMA1 stream 0 memory 0 address register */
#define DMA1_S0M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*0))     /* DMA1 stream 0 memory 1 address register */
#define DMA1_S0FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*0))     /* DMA1 stream 0 FIFO control register */
/* Stream 1 */
#define DMA1_S1CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*1))     /* DMA1 stream 1 configuration register */
#define DMA1_S1NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*1))     /* DMA1 stream 1 number of data register */
#define DMA1_S1PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*1))     /* DMA1 stream 1 peripheral address register */
#define DMA1_S1M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*1))     /* DMA1 stream 1 memory 0 address register */
#define DMA1_S1M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*1))     /* DMA1 stream 1 memory 1 address register */
#define DMA1_S1FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*1))     /* DMA1 stream 1 FIFO control register */
/* Stream 2 */
#define DMA1_S2CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*2))     /* DMA1 stream 2 configuration register */
#define DMA1_S2NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*2))     /* DMA1 stream 2 number of data register */
#define DMA1_S2PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*2))     /* DMA1 stream 2 peripheral address register */
#define DMA1_S2M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*2))     /* DMA1 stream 2 memory 0 address register */
#define DMA1_S2M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*2))     /* DMA1 stream 2 memory 1 address register */
#define DMA1_S2FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*2))     /* DMA1 stream 2 FIFO control register */
/* Stream 3 */
#define DMA1_S3CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*3))     /* DMA1 stream 3 configuration register */
#define DMA1_S3NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*3))     /* DMA1 stream 3 number of data register */
#define DMA1_S3PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*3))     /* DMA1 stream 3 peripheral address register */
#define DMA1_S3M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*3))     /* DMA1 stream 3 memory 0 address register */
#define DMA1_S3M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*3))     /* DMA1 stream 3 memory 1 address register */
#define DMA1_S3FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*3))     /* DMA1 stream 3 FIFO control register */
/* Stream 4 */
#define DMA1_S4CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*4))     /* DMA1 stream 4 configuration register */
#define DMA1_S4NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*4))     /* DMA1 stream 4 number of data register */
#define DMA1_S4PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*4))     /* DMA1 stream 4 peripheral address register */
#define DMA1_S4M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*4))     /* DMA1 stream 4 memory 0 address register */
#define DMA1_S4M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*4))     /* DMA1 stream 4 memory 1 address register */
#define DMA1_S4FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*4))     /* DMA1 stream 4 FIFO control register */
/* Stream 5 */
#define DMA1_S5CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*5))     /* DMA1 stream 5 configuration register */
#define DMA1_S5NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*5))     /* DMA1 stream 5 number of data register */
#define DMA1_S5PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*5))     /* DMA1 stream 5 peripheral address register */
#define DMA1_S5M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*5))     /* DMA1 stream 5 memory 0 address register */
#define DMA1_S5M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*5))     /* DMA1 stream 5 memory 1 address register */
#define DMA1_S5FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*5))     /* DMA1 stream 5 FIFO control register */
/* Stream 6 */
#define DMA1_S6CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*6))     /* DMA1 stream 6 configuration register */
#define DMA1_S6NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*6))     /* DMA1 stream 6 number of data register */
#define DMA1_S6PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*6))     /* DMA1 stream 6 peripheral address register */
#define DMA1_S6M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*6))     /* DMA1 stream 6 memory 0 address register */
#define DMA1_S6M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*6))     /* DMA1 stream 6 memory 1 address register */
#define DMA1_S6FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*6))     /* DMA1 stream 6 FIFO control register */
/* Stream 7 */
#define DMA1_S7CR                       (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*7))     /* DMA1 stream 7 configuration register */
#define DMA1_S7NDTR                     (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*7))     /* DMA1 stream 7 number of data register */
#define DMA1_S7PAR                      (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*7))     /* DMA1 stream 7 peripheral address register */
#define DMA1_S7M0AR                     (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*7))     /* DMA1 stream 7 memory 0 address register */
#define DMA1_S7M1AR                     (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*7))     /* DMA1 stream 7 memory 1 address register */
#define DMA1_S7FCR                      (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*7))     /* DMA1 stream 7 FIFO control register */

/* Direct Memory Access 2 (DMA) */
#define DMA2_LISR                       (volatile uint32_t *) (DMA2_BASE + 0x00)                /* DMA2 low interrupt status register */
#define DMA2_HISR                       (volatile uint32_t *) (DMA2_BASE + 0x04)                /* DMA2 high interrupt status register */
#define DMA2_LIFCR                      (volatile uint32_t *) (DMA2_BASE + 0x08)                /* DMA2 low interrupt flag clear register */
#define DMA2_HIFCR                      (volatile uint32_t *) (DMA2_BASE + 0x0C)                /* DMA2 high interrupt flag clear register */
/* Stream 0 */
#define DMA2_S0CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*0))     /* DMA2 stream 0 configuration register */
#define DMA2_S0NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*0))     /* DMA2 stream 0 number of data register */
#define DMA2_S0PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*0))     /* DMA2 stream 0 peripheral address register */
#define DMA2_S0M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*0))     /* DMA2 stream 0 memory 0 address register */
#define DMA2_S0M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*0))     /* DMA2 stream 0 memory 1 address register */
#define DMA2_S0FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*0))     /* DMA2 stream 0 FIFO control register */
/* Stream 1 */
#define DMA2_S1CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*1))     /* DMA2 stream 1 configuration register */
#define DMA2_S1NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*1))     /* DMA2 stream 1 number of data register */
#define DMA2_S1PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*1))     /* DMA2 stream 1 peripheral address register */
#define DMA2_S1M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*1))     /* DMA2 stream 1 memory 0 address register */
#define DMA2_S1M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*1))     /* DMA2 stream 1 memory 1 address register */
#define DMA2_S1FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*1))     /* DMA2 stream 1 FIFO control register */
/* Stream 2 */
#define DMA2_S2CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*2))     /* DMA2 stream 2 configuration register */
#define DMA2_S2NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*2))     /* DMA2 stream 2 number of data register */
#define DMA2_S2PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*2))     /* DMA2 stream 2 peripheral address register */
#define DMA2_S2M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*2))     /* DMA2 stream 2 memory 0 address register */
#define DMA2_S2M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*2))     /* DMA2 stream 2 memory 1 address register */
#define DMA2_S2FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*2))     /* DMA2 stream 2 FIFO control register */
/* Stream 3 */
#define DMA2_S3CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*3))     /* DMA2 stream 3 configuration register */
#define DMA2_S3NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*3))     /* DMA2 stream 3 number of data register */
#define DMA2_S3PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*3))     /* DMA2 stream 3 peripheral address register */
#define DMA2_S3M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*3))     /* DMA2 stream 3 memory 0 address register */
#define DMA2_S3M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*3))     /* DMA2 stream 3 memory 1 address register */
#define DMA2_S3FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*3))     /* DMA2 stream 3 FIFO control register */
/* Stream 4 */
#define DMA2_S4CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*4))     /* DMA2 stream 4 configuration register */
#define DMA2_S4NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*4))     /* DMA2 stream 4 number of data register */
#define DMA2_S4PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*4))     /* DMA2 stream 4 peripheral address register */
#define DMA2_S4M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*4))     /* DMA2 stream 4 memory 0 address register */
#define DMA2_S4M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*4))     /* DMA2 stream 4 memory 1 address register */
#define DMA2_S4FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*4))     /* DMA2 stream 4 FIFO control register */
/* Stream 5 */
#define DMA2_S5CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*5))     /* DMA2 stream 5 configuration register */
#define DMA2_S5NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*5))     /* DMA2 stream 5 number of data register */
#define DMA2_S5PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*5))     /* DMA2 stream 5 peripheral address register */
#define DMA2_S5M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*5))     /* DMA2 stream 5 memory 0 address register */
#define DMA2_S5M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*5))     /* DMA2 stream 5 memory 1 address register */
#define DMA2_S5FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*5))     /* DMA2 stream 5 FIFO control register */
/* Stream 6 */
#define DMA2_S6CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*6))     /* DMA2 stream 6 configuration register */
#define DMA2_S6NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*6))     /* DMA2 stream 6 number of data register */
#define DMA2_S6PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*6))     /* DMA2 stream 6 peripheral address register */
#define DMA2_S6M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*6))     /* DMA2 stream 6 memory 0 address register */
#define DMA2_S6M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*6))     /* DMA2 stream 6 memory 1 address register */
#define DMA2_S6FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*6))     /* DMA2 stream 6 FIFO control register */
/* Stream 7 */
#define DMA2_S7CR                       (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*7))     /* DMA2 stream 7 configuration register */
#define DMA2_S7NDTR                     (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*7))     /* DMA2 stream 7 number of data register */
#define DMA2_S7PAR                      (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*7))     /* DMA2 stream 7 peripheral address register */
#define DMA2_S7M0AR                     (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*7))     /* DMA2 stream 7 memory 0 address register */
#define DMA2_S7M1AR                     (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*7))     /* DMA2 stream 7 memory 1 address register */
#define DMA2_S7FCR                      (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*7))     /* DMA2 stream 7 FIFO control register */

/* SysTick Timer */
#define SYSTICK_CTL                     (volatile uint32_t *) (SYSTICK_BASE)                    /* Control register for SysTick timer peripheral */
#define SYSTICK_RELOAD                  (volatile uint32_t *) (SYSTICK_BASE + 0x04)             /* Value assumed by timer upon reload */
#define SYSTICK_VAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x08)             /* Current value of timer */
#define SYSTICK_CAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x0C)             /* Calibration settings/value register */

/* Nested Vector Interrupt Controller */
#define NVIC_ISER0                      (volatile uint32_t *) (NVIC_BASE + 0x000)               /* Interrupt set-enable register 0 */
#define NVIC_ISER1                      (volatile uint32_t *) (NVIC_BASE + 0x004)               /* Interrupt set-enable register 1 */
#define NVIC_ISER2                      (volatile uint32_t *) (NVIC_BASE + 0x008)               /* Interrupt set-enable register 2 */
#define NVIC_ICER0                      (volatile uint32_t *) (NVIC_BASE + 0x080)               /* Interrupt clear-enable register 0 */
#define NVIC_ICER1                      (volatile uint32_t *) (NVIC_BASE + 0x084)               /* Interrupt clear-enable register 1 */
#define NVIC_ICER2                      (volatile uint32_t *) (NVIC_BASE + 0x088)               /* Interrupt clear-enable register 2 */
#define NVIC_ISPR0                      (volatile uint32_t *) (NVIC_BASE + 0x100)               /* Interrupt set-pending register 0 */
#define NVIC_ICPR0                      (volatile uint32_t *) (NVIC_BASE + 0x180)               /* Interrupt clear-pending register 0 */
#define NVIC_ICPR1                      (volatile uint32_t *) (NVIC_BASE + 0x184)               /* Interrupt clear-pending register 1 */
#define NVIC_ICPR2                      (volatile uint32_t *) (NVIC_BASE + 0x188)               /* Interrupt clear-pending register 2 */
#define NVIC_IPR0                       (volatile uint32_t *) (NVIC_BASE + 0x300)               /* Interrupt 0 priority register */
#define NVIC_IPR14                      (volatile uint32_t *) (NVIC_BASE + 0x314)               /* Interrupt 14 priority register */

/* System Control Block (SCB) */
#define SCB_ICSR                        (volatile uint32_t *) (SCB_BASE + 0x004)                /* Interrupt Control and State Register */
#define SCB_VTOR                        (volatile uint32_t *) (SCB_BASE + 0x008)                /* Vector Table Offset Register */
#define SCB_CPACR                       (volatile uint32_t *) (SCB_BASE + 0x088)                /* Coprocessor (FPU) Access Control Register */
#define SCB_SHCSR                       (volatile uint32_t *) (SCB_BASE + 0x024)                /* System Handler Control and State Register */
#define SCB_CFSR                        (volatile uint32_t *) (SCB_BASE + 0x028)                /* Configurable fault status register - Describes Usage, Bus, and Memory faults */
#define SCB_HFSR                        (volatile uint32_t *) (SCB_BASE + 0x02C)                /* Hard fault status register - Describes hard fault */
#define SCB_MMFAR                       (volatile uint32_t *) (SCB_BASE + 0x034)                /* Memory management fault address register - Address that caused fault */
#define SCB_BFAR                        (volatile uint32_t *) (SCB_BASE + 0x038)                /* Bus fault address register - Address that caused fault */

/* Memory Protection Unit (MPU) 
 * ST PM0214 (Cortex M4 Programming Manual) pg. 195 */
#define MPU_TYPER                       (volatile uint32_t *) (MPU_BASE + 0x00)                 /* MPU Type Register - Describes HW MPU */
#define MPU_CTRL                        (volatile uint32_t *) (MPU_BASE + 0x04)                 /* MPU Control Register */
#define MPU_RNR                         (volatile uint32_t *) (MPU_BASE + 0x08)                 /* MPU Region Number Register */
#define MPU_RBAR                        (volatile uint32_t *) (MPU_BASE + 0x0C)                 /* MPU Region Base Address Register */
#define MPU_RASR                        (volatile uint32_t *) (MPU_BASE + 0x10)                 /* MPU Region Attribute and Size Register */

/* Floating Point Unit (FPU)            
 * ST PM0214 (Cortex M4 Prog            ramming Manual) pg. 236 */
#define FPU_CCR                         (volatile uint32_t *) (FPU_BASE + 0x04)                 /* FPU Context Control Register */
#define FPU_CAR                         (volatile uint32_t *) (FPU_BASE + 0x08)                 /* FPU Context Address Register */

/**********************************************************************************************************************************************/

/* Bit Masks - See RM0090 Reference Manual for STM32F4 for details */
#define PWR_CR_VOS                      (uint16_t) (0x4000)                                     /* Regulator voltage scaling output selection */

#define RCC_CR_HSEON                    (uint32_t) (0x00010000)                                 /* Enable HSE */
#define RCC_CR_HSERDY                   (uint32_t) (0x00020000)                                 /* HSE Ready */
#define RCC_CR_PLLON                    (uint32_t) (0x01000000)                                 /* Main PLL Enable */
#define RCC_CR_PLLRDY                   (uint32_t) (0x02000000)                                 /* Main PLL clock ready */

#define RCC_CFGR_SW                     (uint32_t) (0x00000003)                                 /* SW[1:0] bits (System clock Switch) */
#define RCC_CFGR_SW_PLL                 (uint32_t) (0x00000002)                                 /* PLL selected as system clock */
#define RCC_CFGR_SWS                    (uint32_t) (0x0000000C)                                 /* SWS[1:0] bits (System Clock Switch Status) */
#define RCC_CFGR_SWS_PLL                (uint32_t) (0x00000008)                                 /* PLL used as system clock */
#define RCC_CFGR_HPRE_DIV1              (uint32_t) (0x00000000)                                 /* SYSCLK not divided (highest frequency) */
#define RCC_CFGR_PPRE2_DIV2             (uint32_t) (0x00008000)                                 /* HCLK divided by 2 */
#define RCC_CFGR_PPRE1_DIV4             (uint32_t) (0x00001400)                                 /* HCLK divided by 4 */

#define RCC_PLLCFGR_PLLSRC_HSE          (uint32_t) (0x00400000)                                 /* HSE oscillator selected as clock entry */

#define RCC_AHB1RSTR_DMA1RST            (uint32_t) (1 << 21)                                    /* DMA1 reset */
#define RCC_AHB1RSTR_DMA2RST            (uint32_t) (1 << 22)                                    /* DMA2 reset */
#define RCC_AHB1ENR_GPIOAEN             (uint32_t) (1 << 0)                                     /* GPIOA clock enable */
#define RCC_AHB1ENR_GPIOBEN             (uint32_t) (1 << 1)                                     /* GPIOB clock enable */
#define RCC_AHB1ENR_GPIOCEN             (uint32_t) (1 << 2)                                     /* GPIOC clock enable */
#define RCC_AHB1ENR_GPIODEN             (uint32_t) (1 << 3)                                     /* GPIOD clock enable */
#define RCC_AHB1ENR_GPIOEEN             (uint32_t) (1 << 4)                                     /* GPIOE clock enable */
#define RCC_AHB1ENR_GPIOFEN             (uint32_t) (1 << 5)                                     /* GPIOF clock enable */
#define RCC_AHB1ENR_GPIOGEN             (uint32_t) (1 << 6)                                     /* GPIOG clock enable */
#define RCC_AHB1ENR_GPIOHEN             (uint32_t) (1 << 7)                                     /* GPIOH clock enable */
#define RCC_AHB1ENR_GPIOIEN             (uint32_t) (1 << 8)                                     /* GPIOI clock enable */
#define RCC_AHB1ENR_DMA1EN              (uint32_t) (1 << 21)                                    /* DMA1 clock enable */
#define RCC_AHB1ENR_DMA2EN              (uint32_t) (1 << 22)                                    /* DMA2 clock enable */
#define RCC_APB1ENR_TIM2EN              (uint32_t) (0x00000001)                                 /* TIM2 clock enable */
#define RCC_APB1ENR_SPI2EN              (uint32_t) (0x00004000)                                 /* SPI2 clock enable */
#define RCC_APB1ENR_PWREN               (uint32_t) (0x10000000)                                 /* Power Interface Clock Enable */
#define RCC_APB2ENR_SPI1EN              (uint32_t) (1 << 12)                                    /* SPI1 Enable */

#define FLASH_ACR_ICEN                  (uint32_t) (0x00000200)                                 /* Instruction Cache Enable */
#define FLASH_ACR_DCEN                  (uint32_t) (0x00000400)                                 /* Data Cache Enable */
#define FLASH_ACR_LATENCY_5WS           (uint32_t) (0x00000005)                                 /* 5 Wait States Latency */

/* TIM2 */
#define TIM2_CR1_CEN                    (uint32_t) (1 << 0)                                     /* TIM2 counter enable */
#define TIM2_CR1_UDIS                   (uint32_t) (1 << 1)                                     /* TIM2 update disable */
#define TIM2_CR1_URS                    (uint32_t) (1 << 2)                                     /* TIM2 update request source */
#define TIM2_CR1_OPM                    (uint32_t) (1 << 3)                                     /* TIM2 one-pulse mode */
#define TIM2_CR1_DIR_DOWN               (uint32_t) (1 << 4)                                     /* TIM2 downcounter */
#define TIM2_CR1_CMS_EDGE               (uint32_t) (0 << 5)                                     /* TIM2 center-aligned mode selection - counter up or down depending on DIR bit */
#define TIM2_CR1_CMS_CM1                (uint32_t) (1 << 5)                                     /* TIM2 center-aligned mode selection - up and down, compare flags set down */
#define TIM2_CR1_CMS_CM2                (uint32_t) (2 << 5)                                     /* TIM2 center-aligned mode selection - up and down, compare flags set up */
#define TIM2_CR1_CMS_CM3                (uint32_t) (3 << 5)                                     /* TIM2 center-aligned mode selection - up and down, compare flags set up/down */
#define TIM2_CR1_ARPE                   (uint32_t) (1 << 7)                                     /* TIM2 auto-reload preload enable */
#define TIM2_CR1_CKD_1                  (uint32_t) (0 << 8)                                     /* TIM2 clock division 1 */
#define TIM2_CR1_CKD_2                  (uint32_t) (1 << 8)                                     /* TIM2 clock division 2 */
#define TIM2_CR1_CKD_4                  (uint32_t) (2 << 8)                                     /* TIM2 clock division 4 */

#define TIM2_CR2_CCDS                   (uint32_t) (1 << 3)                                     /* TIM2 capture/compare DMA requests send when update event occurs */
#define TIM2_CR2_MMS_RST                (uint32_t) (0 << 4)                                     /* TIM2 master mode - reset */
#define TIM2_CR2_MMS_EN                 (uint32_t) (1 << 4)                                     /* TIM2 master mode - enable */
#define TIM2_CR2_MMS_UP                 (uint32_t) (2 << 4)                                     /* TIM2 master mode - update */
#define TIM2_CR2_MMS_CMP_PUL            (uint32_t) (3 << 4)                                     /* TIM2 master mode - compare pulse */
#define TIM2_CR2_MMS_CMP_OC1            (uint32_t) (4 << 4)                                     /* TIM2 master mode - compare OC1 */
#define TIM2_CR2_MMS_CMP_OC2            (uint32_t) (5 << 4)                                     /* TIM2 master mode - compare OC2 */
#define TIM2_CR2_MMS_CMP_OC3            (uint32_t) (6 << 4)                                     /* TIM2 master mode - compare OC3 */
#define TIM2_CR2_MMS_CMP_OC4            (uint32_t) (7 << 4)                                     /* TIM2 master mode - compare OC4 */
#define TIM2_CR2_TI1_123                (uint32_t) (1 << 7)                                     /* TIM2 CH1, CH2, CH3 pins connected to TI1 */

#define TIM2_DIER_UIE                   (uint32_t) (1 << 0)                                     /* TIM2 update interrupt enable */
#define TIM2_DIER_CC1IE                 (uint32_t) (1 << 1)                                     /* TIM2 CC1 interrupt enable */
#define TIM2_DIER_CC2IE                 (uint32_t) (1 << 2)                                     /* TIM2 CC2 interrupt enable */
#define TIM2_DIER_CC3IE                 (uint32_t) (1 << 3)                                     /* TIM2 CC3 interrupt enable */
#define TIM2_DIER_CC4IE                 (uint32_t) (1 << 4)                                     /* TIM2 CC4 interrupt enable */
#define TIM2_DIER_TIE                   (uint32_t) (1 << 6)                                     /* TIM2 trigger interrupt enable */
#define TIM2_DIER_UDE                   (uint32_t) (1 << 8)                                     /* TIM2 update DMA request enable */
#define TIM2_DIER_CC1DE                 (uint32_t) (1 << 9)                                     /* TIM2 CC1 DMA request enable */
#define TIM2_DIER_CC2DE                 (uint32_t) (1 << 10)                                    /* TIM2 CC2 DMA request enable */
#define TIM2_DIER_CC3DE                 (uint32_t) (1 << 11)                                    /* TIM2 CC3 DMA request enable */
#define TIM2_DIER_CC4DE                 (uint32_t) (1 << 12)                                    /* TIM2 CC4 DMA request enable */
#define TIM2_DIER_TDE                   (uint32_t) (1 << 14)                                    /* TIM2 trigger DMA request enable */

/* SPI */
#define SPI_CR1_CPHA                    (uint32_t) (1 << 0)                                     /* SPI clock phase */
#define SPI_CR1_CPOL                    (uint32_t) (1 << 1)                                     /* SPI clock polarity */
#define SPI_CR1_MSTR                    (uint32_t) (1 << 2)                                     /* SPI master selection */
#define SPI_CR1_BR_2                    (uint32_t) (0 << 3)                                     /* SPI baud rate = fPCLK/2 */
#define SPI_CR1_BR_4                    (uint32_t) (1 << 3)                                     /* SPI baud rate = fPCLK/4 */
#define SPI_CR1_BR_8                    (uint32_t) (2 << 3)                                     /* SPI baud rate = fPCLK/8 */
#define SPI_CR1_BR_16                   (uint32_t) (3 << 3)                                     /* SPI baud rate = fPCLK/16 */
#define SPI_CR1_BR_32                   (uint32_t) (4 << 3)                                     /* SPI baud rate = fPCLK/32 */
#define SPI_CR1_BR_64                   (uint32_t) (5 << 3)                                     /* SPI baud rate = fPCLK/64 */
#define SPI_CR1_BR_128                  (uint32_t) (6 << 3)                                     /* SPI baud rate = fPCLK/128 */
#define SPI_CR1_BR_256                  (uint32_t) (7 << 3)                                     /* SPI baud rate = fPCLK/256 */
#define SPI_CR1_SPE                     (uint32_t) (1 << 6)                                     /* SPI enable */
#define SPI_CR1_LSBFIRST                (uint32_t) (1 << 7)                                     /* SPI LSB transmitted first */
#define SPI_CR1_SSI                     (uint32_t) (1 << 8)                                     /* SPI internal slave select */
#define SPI_CR1_SSM                     (uint32_t) (1 << 9)                                     /* SPI software slave management */
#define SPI_CR1_DFF                     (uint32_t) (1 << 11)                                    /* SPI data frame format (0 = 8bit, 1 = 16bit) */
#define SPI_SR_RXNE                     (uint32_t) (1 << 0)                                     /* SPI receive not empty */
#define SPI_SR_TXNE                     (uint32_t) (1 << 1)                                     /* SPI transmit not empty */
#define SPI_SR_CHSIDE                   (uint32_t) (1 << 2)                                     /* SPI channel side */
#define SPI_SR_UDR                      (uint32_t) (1 << 3)                                     /* SPI underrun flag */
#define SPI_SR_CRCERR                   (uint32_t) (1 << 4)                                     /* SPI CRC error flag */
#define SPI_SR_MODF                     (uint32_t) (1 << 5)                                     /* SPI mode fault */
#define SPI_SR_OVR                      (uint32_t) (1 << 6)                                     /* SPI overrun flag */
#define SPI_SR_BSY                      (uint32_t) (1 << 7)                                     /* SPI busy flag */
#define SPI_SR_TIRFE                    (uint32_t) (1 << 8)                                     /* SPI TI frame format error */

/* USART */
#define USART_SR_TC                     (uint32_t) (1 << 6)                                     /* USART Transmission Complete */
#define USART_SR_RXNE                   (uint32_t) (1 << 5)                                     /* USART Read data register not empty */
#define USART_CR1_UE                    (uint32_t) (1 << 13)                                    /* USART Enable */
#define USART_CR1_RXNEIE                (uint32_t) (1 << 5)                                     /* RXNE Interrupt Enable */
#define USART_CR1_TE                    (uint32_t) (1 << 3)                                     /* USART Transmit Enable */
#define USART_CR1_RE                    (uint32_t) (1 << 2)                                     /* USART Receive Enable */
#define USART_CR3_DMAR_EN               (uint32_t) (1 << 6)                                     /* USART DMA Receive Enable */
#define USART_CR3_DMAT_EN               (uint32_t) (1 << 7)                                     /* USART DMA Transmit Enable */

/* GPIO */
#define GPIO_MODER_OUT                  (uint32_t) (0x1)                                        /* Sets GPIO pin to output mode */
#define GPIO_MODER_ALT                  (uint32_t) (0x2)                                        /* Sets GPIO pin to alternative function mode */
#define GPIO_AF_USART13                 (uint32_t) (0x7)                                        /* GPIO USART1-3 mode */
#define GPIO_AF_SPI12                   (uint32_t) (0x5)                                        /* GPIO SPI1-2 mode */

/* DMA */
#define DMA_LISR_TCIF2                  (uint32_t) (1 << 21)                                    /* DMA stream 2 transfer complete flag */
#define DMA_HISR_TCIF7                  (uint32_t) (1 << 27)                                    /* DMA stream 7 transfer complete flag */
#define DMA_LIFCR_CTCIF2                (uint32_t) (1 << 21)                                    /* DMA clear stream 2 transfer complete flag */
#define DMA_HIFCR_CTCIF7                (uint32_t) (1 << 27)                                    /* DMA clear stream 7 transfer complete flag */

#define DMA_SxCR_EN                     (uint32_t) (1 << 0)                                     /* DMA stream enable */
#define DMA_SxCR_DMEIE                  (uint32_t) (1 << 1)                                     /* DMA stream direct mode error interrupt enable */
#define DMA_SxCR_TEIE                   (uint32_t) (1 << 2)                                     /* DMA stream transmit error interrupt enable */
#define DMA_SxCR_HTIE                   (uint32_t) (1 << 3)                                     /* DMA stream half transfer interrupt enable */
#define DMA_SxCR_TCIE                   (uint32_t) (1 << 4)                                     /* DMA stream transfer complete interrupt enable */
#define DMA_SxCR_PFCTRL                 (uint32_t) (1 << 5)                                     /* DMA stream peripheral flow control */
#define DMA_SxCR_DIR_PM                 (uint32_t) (0 << 6)                                     /* DMA stream peripheral-to-memory data transfer */
#define DMA_SxCR_DIR_MP                 (uint32_t) (1 << 6)                                     /* DMA stream memory-to-peripheral data transfer */
#define DMA_SxCR_DIR_MM                 (uint32_t) (2 << 6)                                     /* DMA stream memory-to-memory data transfer */
#define DMA_SxCR_CIRC                   (uint32_t) (1 << 8)                                     /* DMA stream circular mode enable */
#define DMA_SxCR_PINC                   (uint32_t) (1 << 9)                                     /* DMA stream peripheral increment mode enable */
#define DMA_SxCR_MINC                   (uint32_t) (1 << 10)                                    /* DMA stream memory increment mode enable */
#define DMA_SxCR_PSIZE_BYTE             (uint32_t) (0 << 11)                                    /* DMA stream peripheral data size - Byte */
#define DMA_SxCR_PSIZE_HW               (uint32_t) (1 << 11)                                    /* DMA stream peripheral data size - Half-word */
#define DMA_SxCR_PSIZE_WORD             (uint32_t) (2 << 11)                                    /* DMA stream peripheral data size - Word */
#define DMA_SxCR_MSIZE_BYTE             (uint32_t) (0 << 13)                                    /* DMA stream memory data size - Byte */
#define DMA_SxCR_MSIZE_HW               (uint32_t) (1 << 13)                                    /* DMA stream memory data size - Half-word */
#define DMA_SxCR_MSIZE_WORD             (uint32_t) (2 << 13)                                    /* DMA stream memory data size - Word */
#define DMA_SxCR_PINCOS_LINKED          (uint32_t) (0 << 15)                                    /* DMA stream peripheral increment offset size linked to PSIZE */
#define DMA_SxCR_PINCOS_FIXED           (uint32_t) (1 << 15)                                    /* DMA stream peripheral increment offset size fixed */
#define DMA_SxCR_PL_LOW                 (uint32_t) (0 << 16)                                    /* DMA stream priority level low */
#define DMA_SxCR_PL_MED                 (uint32_t) (1 << 16)                                    /* DMA stream priority level medium */
#define DMA_SxCR_PL_HIGH                (uint32_t) (2 << 16)                                    /* DMA stream priority level high */
#define DMA_SxCR_PL_VHIGH               (uint32_t) (3 << 16)                                    /* DMA stream priority level very high */
#define DMA_SxCR_DBM                    (uint32_t) (1 << 18)                                    /* DMA stream double buffer mode */
#define DMA_SxCR_CT                     (uint32_t) (1 << 19)                                    /* DMA stream current target (M0 or M1) */
#define DMA_SxCR_PBURST_NO              (uint32_t) (0 << 21)                                    /* DMA stream peripheral burst disable */
#define DMA_SxCR_PBURST_4               (uint32_t) (1 << 21)                                    /* DMA stream peripheral burst of 4 beats */
#define DMA_SxCR_PBURST_8               (uint32_t) (2 << 21)                                    /* DMA stream peripheral burst of 8 beats */
#define DMA_SxCR_PBURST_16              (uint32_t) (3 << 21)                                    /* DMA stream peripheral burst of 16 beats */
#define DMA_SxCR_MBURST_NO              (uint32_t) (0 << 23)                                    /* DMA stream memory burst disable */
#define DMA_SxCR_MBURST_4               (uint32_t) (1 << 23)                                    /* DMA stream memory burst of 4 beats */
#define DMA_SxCR_MBURST_8               (uint32_t) (2 << 23)                                    /* DMA stream memory burst of 8 beats */
#define DMA_SxCR_MBURST_16              (uint32_t) (3 << 23)                                    /* DMA stream memory burst of 16 beats */
#define DMA_SxCR_CHSEL(x)               (uint32_t) (x << 25)                                    /* DMA stream channel select */

#define DMA_SxFCR_FTH_1                 (uint32_t) (0 << 0)                                     /* DMA stream FIFO threshold 1/4 */
#define DMA_SxFCR_FTH_2                 (uint32_t) (1 << 0)                                     /* DMA stream FIFO threshold 1/2 */
#define DMA_SxFCR_FTH_3                 (uint32_t) (2 << 0)                                     /* DMA stream FIFO threshold 3/4 */
#define DMA_SxFCR_FTH_4                 (uint32_t) (3 << 0)                                     /* DMA stream FIFO threshold full*/
#define DMA_SxFCR_DMDIS                 (uint32_t) (1 << 2)                                     /* DMA stream direct mode disable */
#define DMA_SxFCR_FS                    (uint32_t) (7 << 3)                                     /* DMA stream FIFO status */
#define DMA_SxFCR_FEIE                  (uint32_t) (1 << 7)                                     /* DMA stream FIFO error interrupt enable */

/* System Control Block */
#define SCB_ICSR_PENDSVCLR              (uint32_t) (1 << 27)                                    /* Clear PendSV interrupt */
#define SCB_ICSR_PENDSVSET              (uint32_t) (1 << 28)                                    /* Set PendSV interrupt */
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

/* Floating Point Unit (FPU)
 * ST PM0214 (Cortex M4 Programming Manual) pg. 236 */
#define FPU_CCR_ASPEN                   (uint32_t) (1 << 31)                                    /* FPU Automatic State Preservation */
