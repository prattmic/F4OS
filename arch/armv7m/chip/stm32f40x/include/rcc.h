/*
 * Copyright (C) 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ARCH_CHIP_RCC_H_INCLUDED
#define ARCH_CHIP_RCC_H_INCLUDED

#include <stdint.h>
#include <arch/chip/periph.h>
#include <arch/chip/registers.h>

enum stm32f4_bus {
    STM32F4_AHB1,
    STM32F4_AHB2,
    STM32F4_AHB3,
    STM32F4_APB1,
    STM32F4_APB2,
    STM32F4_UNKNOWN_BUS = -1,
};

struct stm32f4_rcc_regs {
    uint32_t CR;        /* Clock control register */
    uint32_t PLLCFGR;   /* PLL configuration register */
    uint32_t CFGR;      /* Clock configuration register */
    uint32_t CIR;       /* Clock interrupt register */
    uint32_t AHB1RSTR;  /* AHB1 reset register */
    uint32_t AHB2RSTR;  /* AHB2 reset register */
    uint32_t AHB3RSTR;  /* AHB3 reset register */
    uint32_t reserved1;
    uint32_t APB1RSTR;  /* APB1 reset register */
    uint32_t APB2RSTR;  /* APB2 reset register */
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t AHB1ENR;   /* AHB1 enable register */
    uint32_t AHB2ENR;   /* AHB2 enable register */
    uint32_t AHB3ENR;   /* AHB3 enable register */
    uint32_t reserved4;
    uint32_t APB1ENR;   /* APB1 peripheral clock enable register */
    uint32_t APB2ENR;   /* APB2 peripheral clock enable register */
    uint32_t reserved5;
    uint32_t reserved6;
    uint32_t AHB1LPENR; /* AHB1 low power enable register */
    uint32_t AHB2LPENR; /* AHB2 low power enable register */
    uint32_t AHB3LPENR; /* AHB3 low power enable register */
    uint32_t reserved7;
    uint32_t APB1LPENR; /* APB1 peripheral clock low power enable register */
    uint32_t APB2LPENR; /* APB2 peripheral clock low power enable register */
    uint32_t reserved8;
    uint32_t reserved9;
    uint32_t BDCR;      /* Backup domain control register */
    uint32_t CSR;       /* Clock control and status register */
    uint32_t reserved10;
    uint32_t reserved11;
    uint32_t SSCGR;     /* Spread spectrum clock generation register */
    uint32_t PLLI2SCFG; /* PLLI2S configuration register */
};

static inline struct stm32f4_rcc_regs *rcc_get_regs() {
    return (struct stm32f4_rcc_regs *) RCC_BASE;
}

/*
 * Enable/disable peripheral clock
 *
 * Enables or disables the clock for the specified peripheral.  This covers
 * any periphal clocks covered by RCC.
 *
 * @param periphid  Peripheral ID to enable/disable
 * @param enable    1 to enable clocks, 0 to disable
 * @returns 0 on success, negative on error
 */
int rcc_set_clock_enable(enum stm32f4_periph_id periphid, int enable);

/*
 * Lookup peripheral bus
 *
 * @param periphid  Periipheral to look up
 * @returns system bus which the peripheral is connected to.
 */
enum stm32f4_bus rcc_peripheral_bus(enum stm32f4_periph_id periphid);

/*
 * Lookup bus clock
 *
 * @param bus   Bus to look up
 * @returns bus clock speed in Hz, negative on error
 */
long rcc_bus_clock(enum stm32f4_bus bus);

#define RCC_CR                      ((volatile uint32_t *) (RCC_BASE + 0x00))   /* Clock Control Register */
#define RCC_PLLCFGR                 ((volatile uint32_t *) (RCC_BASE + 0x04))   /* PLL Configuration Register */
#define RCC_CFGR                    ((volatile uint32_t *) (RCC_BASE + 0x08))   /* Clock Configuration Register */
#define RCC_CIR                     ((volatile uint32_t *) (RCC_BASE + 0x0C))   /* Clock Interrupt Register */
#define RCC_AHB1RSTR                ((volatile uint32_t *) (RCC_BASE + 0x10))   /* AHB1 reset Register */
#define RCC_AHB1ENR                 ((volatile uint32_t *) (RCC_BASE + 0x30))   /* AHB1 Enable Register */
#define RCC_AHB2ENR                 ((volatile uint32_t *) (RCC_BASE + 0x34))   /* AHB2 Enable Register */
#define RCC_AHB3ENR                 ((volatile uint32_t *) (RCC_BASE + 0x38))   /* AHB3 Enable Register */
#define RCC_APB1ENR                 ((volatile uint32_t *) (RCC_BASE + 0x40))   /* APB1 Peripheral Clock Enable Register */
#define RCC_APB2ENR                 ((volatile uint32_t *) (RCC_BASE + 0x44))   /* APB2 Peripheral Clock Enable Register */

#define RCC_CR_HSION                ((uint32_t) (1 << 0))       /* HSI clock enable */
#define RCC_CR_HSIRDY               ((uint32_t) (1 << 1))       /* HSI ready */
#define RCC_CR_HSITRIM_M            ((uint32_t) (0x1F << 3))    /* HSI trimming mask */
#define RCC_CR_HSITRIM(n)           ((uint32_t)) (n << 3)       /* HSI trimming */
#define RCC_CR_HSICAL_M             ((uint32_t) (0xFF << 8))    /* HSI calibration mask */
#define RCC_CR_HSEON                ((uint32_t) (1 << 16))      /* HSE clock enable */
#define RCC_CR_HSERDY               ((uint32_t) (1 << 17))      /* HSE ready */
#define RCC_CR_HSEBYP               ((uint32_t) (1 << 18))      /* HSE bypass */
#define RCC_CR_CSSON                ((uint32_t) (1 << 19))      /* Clock security system enable */
#define RCC_CR_PLLON                ((uint32_t) (1 << 24))      /* Main PLL enable */
#define RCC_CR_PLLRDY               ((uint32_t) (1 << 25))      /* Main PLL clock ready */
#define RCC_CR_PLLI2SON             ((uint32_t) (1 << 26))      /* PLLI2S enable */
#define RCC_CR_PLLI2SRDY            ((uint32_t) (1 << 27))      /* PLLI2S clock ready */

#define RCC_PLLCFGR_RESET           ((uint32_t) (0x24003010))   /* PLLCFGR register reset value */
#define RCC_PLLCFGR_PLLSRC_HSE      ((uint32_t) (1 << 22))      /* HSE oscillator selected as clock entry */

#define RCC_CFGR_SW_M               ((uint32_t) (3 << 0))       /* System clock switch mask */
#define RCC_CFGR_SW_HSI             ((uint32_t) (0 << 0))       /* System clock switch - HSI selected as system clock */
#define RCC_CFGR_SW_PLL             ((uint32_t) (1 << 0))       /* System clock switch - HSE selected as system clock */
#define RCC_CFGR_SW_PLL             ((uint32_t) (2 << 0))       /* System clock switch - PLL selected as system clock */
#define RCC_CFGR_SWS_M              ((uint32_t) (3 << 2))       /* System clock switch status mask */
#define RCC_CFGR_SWS_HSI            ((uint32_t) (0 << 2))       /* System clock switch status - HSI used as system clock */
#define RCC_CFGR_SWS_HSE            ((uint32_t) (1 << 2))       /* System clock switch status - HSE used as system clock */
#define RCC_CFGR_SWS_PLL            ((uint32_t) (2 << 2))       /* System clock switch status - PLL used as system clock */
#define RCC_CFGR_HPRE_M             ((uint32_t) (0xF << 4))     /* AHB prescaler mask */
#define RCC_CFGR_HPRE_DIV1          ((uint32_t) (0x0 << 4))     /* AHB prescaler - SYSCLK not divided */
#define RCC_CFGR_HPRE_DIV2          ((uint32_t) (0x8 << 4))     /* AHB prescaler - SYSCLK/2 */
#define RCC_CFGR_HPRE_DIV4          ((uint32_t) (0x9 << 4))     /* AHB prescaler - SYSCLK/4 */
#define RCC_CFGR_HPRE_DIV8          ((uint32_t) (0xA << 4))     /* AHB prescaler - SYSCLK/8 */
#define RCC_CFGR_HPRE_DIV16         ((uint32_t) (0xB << 4))     /* AHB prescaler - SYSCLK/16 */
#define RCC_CFGR_HPRE_DIV64         ((uint32_t) (0xC << 4))     /* AHB prescaler - SYSCLK/64 */
#define RCC_CFGR_HPRE_DIV128        ((uint32_t) (0xD << 4))     /* AHB prescaler - SYSCLK/128 */
#define RCC_CFGR_HPRE_DIV256        ((uint32_t) (0xE << 4))     /* AHB prescaler - SYSCLK/256 */
#define RCC_CFGR_HPRE_DIV512        ((uint32_t) (0xF << 4))     /* AHB prescaler - SYSCLK/512 */
#define RCC_CFGR_PPRE1_M            ((uint32_t) (7 << 10))      /* APB low speed prescaler mask */
#define RCC_CFGR_PPRE1_DIV1         ((uint32_t) (0 << 10))      /* APB low speed prescaler - HCLK/1 */
#define RCC_CFGR_PPRE1_DIV2         ((uint32_t) (4 << 10))      /* APB low speed prescaler - HCLK/2 */
#define RCC_CFGR_PPRE1_DIV4         ((uint32_t) (5 << 10))      /* APB low speed prescaler - HCLK/4 */
#define RCC_CFGR_PPRE1_DIV8         ((uint32_t) (6 << 10))      /* APB low speed prescaler - HCLK/8 */
#define RCC_CFGR_PPRE1_DIV16        ((uint32_t) (7 << 10))      /* APB low speed prescaler - HCLK/16 */
#define RCC_CFGR_PPRE2_M            ((uint32_t) (7 << 13))      /* APB high speec prescaler mask */
#define RCC_CFGR_PPRE2_DIV1         ((uint32_t) (0 << 13))      /* APB high speed prescaler - HCLK/1 */
#define RCC_CFGR_PPRE2_DIV2         ((uint32_t) (4 << 13))      /* APB high speed prescaler - HCLK/2 */
#define RCC_CFGR_PPRE2_DIV4         ((uint32_t) (5 << 13))      /* APB high speed prescaler - HCLK/4 */
#define RCC_CFGR_PPRE2_DIV8         ((uint32_t) (6 << 13))      /* APB high speed prescaler - HCLK/8 */
#define RCC_CFGR_PPRE2_DIV16        ((uint32_t) (7 << 13))      /* APB high speed prescaler - HCLK/16 */
#define RCC_CFGR_RTCPRE_M           ((uint32_t) (0x1F << 16))   /* HSE division factor for RTC clock mask */
#define RCC_CFGR_RTCPRE(n)          ((uint32_t)) (n << 16)      /* HSE division factor for RTC clock */

#define RCC_AHB1RSTR_GPIOARST       ((uint32_t) (1 << 0))       /* GPIOA reset */
#define RCC_AHB1RSTR_GPIOBRST       ((uint32_t) (1 << 1))       /* GPIOB reset */
#define RCC_AHB1RSTR_GPIOCRST       ((uint32_t) (1 << 2))       /* GPIOC reset */
#define RCC_AHB1RSTR_GPIODRST       ((uint32_t) (1 << 3))       /* GPIOD reset */
#define RCC_AHB1RSTR_GPIOERST       ((uint32_t) (1 << 4))       /* GPIOE reset */
#define RCC_AHB1RSTR_GPIOFRST       ((uint32_t) (1 << 5))       /* GPIOF reset */
#define RCC_AHB1RSTR_GPIOGRST       ((uint32_t) (1 << 6))       /* GPIOG reset */
#define RCC_AHB1RSTR_GPIOHRST       ((uint32_t) (1 << 7))       /* GPIOH reset */
#define RCC_AHB1RSTR_GPIOIRST       ((uint32_t) (1 << 8))       /* GPIOI reset */
#define RCC_AHB1RSTR_CRCRST         ((uint32_t) (1 << 12))      /* CRC reset */
#define RCC_AHB1RSTR_DMA1RST        ((uint32_t) (1 << 21))      /* DMA1 reset */
#define RCC_AHB1RSTR_DMA2RST        ((uint32_t) (1 << 22))      /* DMA2 reset */
#define RCC_AHB1RSTR_ETHMACRST      ((uint32_t) (1 << 25))      /* Ethernet MAC reset */
#define RCC_AHB1RSTR_OTGHSRST       ((uint32_t) (1 << 29))      /* USB OTG HS reset */

#define RCC_AHB2RSTR_DCMIRST        ((uint32_t) (1 << 0))       /* Camera interface reset */
#define RCC_AHB2RSTR_CRYPRST        ((uint32_t) (1 << 4))       /* Cyrpto modules reset */
#define RCC_AHB2RSTR_HASHRST        ((uint32_t) (1 << 5))       /* Hash modules reset */
#define RCC_AHB2RSTR_RNGRST         ((uint32_t) (1 << 6))       /* Random number generator reset */
#define RCC_AHB2RSTR_OTGFSRST       ((uint32_t) (1 << 7))       /* USB OTG FS reset */

#define RCC_AHB3RSTR_FSMCRST        ((uint32_t) (1 << 0))       /* Flexible static memeory controller reset */

#define RCC_APB1RSTR_TIM2RST        ((uint32_t) (1 << 0))       /* TIM2 reset */
#define RCC_APB1RSTR_TIM3RST        ((uint32_t) (1 << 1))       /* TIM3 reset */
#define RCC_APB1RSTR_TIM4RST        ((uint32_t) (1 << 2))       /* TIM4 reset */
#define RCC_APB1RSTR_TIM5RST        ((uint32_t) (1 << 3))       /* TIM5 reset */
#define RCC_APB1RSTR_TIM6RST        ((uint32_t) (1 << 4))       /* TIM6 reset */
#define RCC_APB1RSTR_TIM7RST        ((uint32_t) (1 << 5))       /* TIM7 reset */
#define RCC_APB1RSTR_TIM12RST       ((uint32_t) (1 << 6))       /* TIM12 reset */
#define RCC_APB1RSTR_TIM13RST       ((uint32_t) (1 << 7))       /* TIM13 reset */
#define RCC_APB1RSTR_TIM14RST       ((uint32_t) (1 << 8))       /* TIM14 reset */
#define RCC_APB1RSTR_WWDGRST        ((uint32_t) (1 << 11))      /* Window watchdog reset */
#define RCC_APB1RSTR_SPI2RST        ((uint32_t) (1 << 14))      /* SPI2 reset */
#define RCC_APB1RSTR_SPI3RST        ((uint32_t) (1 << 15))      /* SPI3 reset */
#define RCC_APB1RSTR_USART2RST      ((uint32_t) (1 << 17))      /* USART2 reset */
#define RCC_APB1RSTR_USART3RST      ((uint32_t) (1 << 18))      /* USART3 reset */
#define RCC_APB1RSTR_USART4RST      ((uint32_t) (1 << 19))      /* USART4 reset */
#define RCC_APB1RSTR_USART5RST      ((uint32_t) (1 << 20))      /* USART5 reset */
#define RCC_APB1RSTR_I2C1RST        ((uint32_t) (1 << 21))      /* I2C1 reset */
#define RCC_APB1RSTR_I2C2RST        ((uint32_t) (1 << 22))      /* I2C2 reset */
#define RCC_APB1RSTR_I2C3RST        ((uint32_t) (1 << 23))      /* I2C3 reset */
#define RCC_APB1RSTR_CAN1RST        ((uint32_t) (1 << 25))      /* CAN1 reset */
#define RCC_APB1RSTR_CAN2RST        ((uint32_t) (1 << 26))      /* CAN2 reset */
#define RCC_APB1RSTR_PWRRST         ((uint32_t) (1 << 28))      /* Power interface reset */
#define RCC_APB1RSTR_DACRST         ((uint32_t) (1 << 29))      /* DAC reset */

#define RCC_APB2RSTR_TIM1RST        ((uint32_t) (1 << 0))       /* TIM1 reset */
#define RCC_APB2RSTR_TIM8RST        ((uint32_t) (1 << 1))       /* TIM8 reset */
#define RCC_APB2RSTR_USART1RST      ((uint32_t) (1 << 4))       /* USART1 reset */
#define RCC_APB2RSTR_USART6RST      ((uint32_t) (1 << 5))       /* USART6 reset */
#define RCC_APB2RSTR_ADCRST         ((uint32_t) (1 << 8))       /* ADC1 reset */
#define RCC_APB2RSTR_SDIORST        ((uint32_t) (1 << 11))      /* SDIO reset */
#define RCC_APB2RSTR_SPI1RST        ((uint32_t) (1 << 12))      /* SPI1 reset */
#define RCC_APB2RSTR_SYSCFGRST      ((uint32_t) (1 << 14))      /* System configuration controller reset */
#define RCC_APB2RSTR_TIM9RST        ((uint32_t) (1 << 16))      /* TIM9 reset */
#define RCC_APB2RSTR_TIM10RST       ((uint32_t) (1 << 17))      /* TIM10 reset */
#define RCC_APB2RSTR_TIM11RST       ((uint32_t) (1 << 18))      /* TIM11 reset */

#define RCC_AHB1ENR_GPIOAEN         ((uint32_t) (1 << 0))       /* GPIOA clock enable */
#define RCC_AHB1ENR_GPIOBEN         ((uint32_t) (1 << 1))       /* GPIOB clock enable */
#define RCC_AHB1ENR_GPIOCEN         ((uint32_t) (1 << 2))       /* GPIOC clock enable */
#define RCC_AHB1ENR_GPIODEN         ((uint32_t) (1 << 3))       /* GPIOD clock enable */
#define RCC_AHB1ENR_GPIOEEN         ((uint32_t) (1 << 4))       /* GPIOE clock enable */
#define RCC_AHB1ENR_GPIOFEN         ((uint32_t) (1 << 5))       /* GPIOF clock enable */
#define RCC_AHB1ENR_GPIOGEN         ((uint32_t) (1 << 6))       /* GPIOG clock enable */
#define RCC_AHB1ENR_GPIOHEN         ((uint32_t) (1 << 7))       /* GPIOH clock enable */
#define RCC_AHB1ENR_GPIOIEN         ((uint32_t) (1 << 8))       /* GPIOI clock enable */
#define RCC_AHB1ENR_CRCEN           ((uint32_t) (1 << 12))      /* CRC clock enable */
#define RCC_AHB1ENR_BKPSRAMEN       ((uint32_t) (1 << 18))      /* Backup SRAM clock enable */
#define RCC_AHB1ENR_CCMDATARAMEN    ((uint32_t) (1 << 20))      /* CCM data RAM clock enable */
#define RCC_AHB1ENR_DMA1EN          ((uint32_t) (1 << 21))      /* DMA1 clock enable */
#define RCC_AHB1ENR_DMA2EN          ((uint32_t) (1 << 22))      /* DMA2 clock enable */
#define RCC_AHB1ENR_ETHMACEN        ((uint32_t) (1 << 25))      /* Ethernet MAC clock enable */
#define RCC_AHB1ENR_ETHMACTXEN      ((uint32_t) (1 << 26))      /* Ethernet MAC TX clock enable */
#define RCC_AHB1ENR_ETHMACRXEN      ((uint32_t) (1 << 27))      /* Ethernet MAC RX clock enable */
#define RCC_AHB1ENR_ETHMACPTPEN     ((uint32_t) (1 << 28))      /* Ethernet MAC PTP clock enable */
#define RCC_AHB1ENR_OTGHSEN         ((uint32_t) (1 << 29))      /* USB OTG HS clock enable */
#define RCC_AHB1ENR_OTGHSULPIEN     ((uint32_t) (1 << 30))      /* USB OTG HSULPI clock enable */

#define RCC_AHB2ENR_DCMIEN          ((uint32_t) (1 << 0))       /* Camera interface clock enable */
#define RCC_AHB2ENR_CRYPEN          ((uint32_t) (1 << 4))       /* Cyrpto modules clock enable */
#define RCC_AHB2ENR_HASHEN          ((uint32_t) (1 << 5))       /* Hash modules clock enable */
#define RCC_AHB2ENR_RNGEN           ((uint32_t) (1 << 6))       /* Random number generator clock enable */
#define RCC_AHB2ENR_OTGFSEN         ((uint32_t) (1 << 7))       /* USB OTG FS clock enable */

#define RCC_AHB3ENR_FSMCEN          ((uint32_t) (1 << 0))       /* Flexible static memeory controller clock enable */

#define RCC_APB1ENR_TIM2EN          ((uint32_t) (1 << 0))       /* TIM2 clock enable */
#define RCC_APB1ENR_TIM3EN          ((uint32_t) (1 << 1))       /* TIM3 clock enable */
#define RCC_APB1ENR_TIM4EN          ((uint32_t) (1 << 2))       /* TIM4 clock enable */
#define RCC_APB1ENR_TIM5EN          ((uint32_t) (1 << 3))       /* TIM5 clock enable */
#define RCC_APB1ENR_TIM6EN          ((uint32_t) (1 << 4))       /* TIM6 clock enable */
#define RCC_APB1ENR_TIM7EN          ((uint32_t) (1 << 5))       /* TIM7 clock enable */
#define RCC_APB1ENR_TIM12EN         ((uint32_t) (1 << 6))       /* TIM12 clock enable */
#define RCC_APB1ENR_TIM13EN         ((uint32_t) (1 << 7))       /* TIM13 clock enable */
#define RCC_APB1ENR_TIM14EN         ((uint32_t) (1 << 8))       /* TIM14 clock enable */
#define RCC_APB1ENR_WWDGEN          ((uint32_t) (1 << 11))      /* Window watchdog clock enable */
#define RCC_APB1ENR_SPI2EN          ((uint32_t) (1 << 14))      /* SPI2 clock enable */
#define RCC_APB1ENR_SPI3EN          ((uint32_t) (1 << 15))      /* SPI3 clock enable */
#define RCC_APB1ENR_USART2EN        ((uint32_t) (1 << 17))      /* USART2 clock enable */
#define RCC_APB1ENR_USART3EN        ((uint32_t) (1 << 18))      /* USART3 clock enable */
#define RCC_APB1ENR_USART4EN        ((uint32_t) (1 << 19))      /* USART4 clock enable */
#define RCC_APB1ENR_USART5EN        ((uint32_t) (1 << 20))      /* USART5 clock enable */
#define RCC_APB1ENR_I2C1EN          ((uint32_t) (1 << 21))      /* I2C1 clock enable */
#define RCC_APB1ENR_I2C2EN          ((uint32_t) (1 << 22))      /* I2C2 clock enable */
#define RCC_APB1ENR_I2C3EN          ((uint32_t) (1 << 23))      /* I2C3 clock enable */
#define RCC_APB1ENR_CAN1EN          ((uint32_t) (1 << 25))      /* CAN1 clock enable */
#define RCC_APB1ENR_CAN2EN          ((uint32_t) (1 << 26))      /* CAN2 clock enable */
#define RCC_APB1ENR_PWREN           ((uint32_t) (1 << 28))      /* Power interface clock enable */
#define RCC_APB1ENR_DACEN           ((uint32_t) (1 << 29))      /* DAC clock enable */

#define RCC_APB2ENR_TIM1EN          ((uint32_t) (1 << 0))       /* TIM1 clock enable */
#define RCC_APB2ENR_TIM8EN          ((uint32_t) (1 << 1))       /* TIM8 clock enable */
#define RCC_APB2ENR_USART1EN        ((uint32_t) (1 << 4))       /* USART1 clock enable */
#define RCC_APB2ENR_USART6EN        ((uint32_t) (1 << 5))       /* USART6 clock enable */
#define RCC_APB2ENR_ADC1EN          ((uint32_t) (1 << 8))       /* ADC1 clock enable */
#define RCC_APB2ENR_ADC2EN          ((uint32_t) (1 << 9))       /* ADC2 clock enable */
#define RCC_APB2ENR_ADC3EN          ((uint32_t) (1 << 10))      /* ADC3 clock enable */
#define RCC_APB2ENR_SDIOEN          ((uint32_t) (1 << 11))      /* SDIO clock enable */
#define RCC_APB2ENR_SPI1EN          ((uint32_t) (1 << 12))      /* SPI1 clock enable */
#define RCC_APB2ENR_SPI4EN          ((uint32_t) (1 << 13))      /* SPI4 clock enable */
#define RCC_APB2ENR_SYSCFGEN        ((uint32_t) (1 << 14))      /* System configuration controller clock enable */
#define RCC_APB2ENR_TIM9EN          ((uint32_t) (1 << 16))      /* TIM9 clock enable */
#define RCC_APB2ENR_TIM10EN         ((uint32_t) (1 << 17))      /* TIM10 clock enable */
#define RCC_APB2ENR_TIM11EN         ((uint32_t) (1 << 18))      /* TIM11 clock enable */
#define RCC_APB2ENR_SPI5EN          ((uint32_t) (1 << 20))      /* SPI5 clock enable */
#define RCC_APB2ENR_SPI6EN          ((uint32_t) (1 << 21))      /* SPI6 clock enable */

#define RCC_AHB3ENR_FSMCEN          ((uint32_t) (1 << 0))       /* FSMC clock enable */

#endif
