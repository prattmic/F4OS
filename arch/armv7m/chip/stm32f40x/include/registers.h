/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

/* STM32F4 Registers and Memory Locations */

#ifndef ARCH_CHIP_REGISTERS_H_INCLUDED
#define ARCH_CHIP_REGISTERS_H_INCLUDED

#include <stdint.h>

/* Memory Map */
#define MEMORY_BASE                     (uint32_t) (0x00000000)                                 /* Base of memory map */
#define FLASH_BASE                      (uint32_t) (0x08000000)                                 /* Flash Memory Base Address */
#define RAM_BASE                        (uint32_t) (0x20000000)                                 /* RAM Base Address */
#define ETHRAM_BASE                     (uint32_t) (0x2001C000)                                 /* ETHRAM Base Address */
#define CCMRAM_BASE                     (uint32_t) (0x10000000)                                 /* CCMRAM Base Address - Accessible only to CPU */

/* Peripheral Map */
#define PERIPH_BASE                     (uint32_t) (0x40000000)                                 /* Peripheral base address */
#define PRIV_PERIPH_BASE                (uint32_t) (0xED000000)                                 /* Private peripheral base address */
#define AHB2PERIPH_BASE                 (PERIPH_BASE + 0x10000000)
#define AHB1PERIPH_BASE                 (PERIPH_BASE + 0x00020000)
#define APB2PERIPH_BASE                 (PERIPH_BASE + 0x00010000)
#define APB1PERIPH_BASE                 (PERIPH_BASE)
#define INVALID_PERIPH_BASE             (uint32_t) (0x0)                                        /* Placeholder for illegal peripherals */
/* AHB2 Peripherals */
#define USB_FS_BASE                     (AHB2PERIPH_BASE + 0x0000)                              /* USB OTG FS base address */
/* AHB1 Peripherals */
#define DMA2_BASE                       (AHB1PERIPH_BASE + 0x6400)                              /* DMA2 base address */
#define DMA1_BASE                       (AHB1PERIPH_BASE + 0x6000)                              /* DMA1 base address */
#define FLASH_R_BASE                    (AHB1PERIPH_BASE + 0x3C00)                              /* Flash registers base address */
#define RCC_BASE                        (AHB1PERIPH_BASE + 0x3800)                              /* Reset and Clock Control base address */
#define GPIO_BASE(port)                 (AHB1PERIPH_BASE + (0x400*port))                        /* GPIO Port base address */
/* APB1 Peripherals */
#define UART8_BASE                      (APB1PERIPH_BASE + 0x7C00)                              /* UART8 Base Address */
#define UART7_BASE                      (APB1PERIPH_BASE + 0x7800)                              /* UART7 Base Address */
#define PWR_BASE                        (APB1PERIPH_BASE + 0x7000)                              /* Power Control base address */
#define I2C3_BASE                       (APB1PERIPH_BASE + 0x5C00)                              /* I2C3 base address */
#define I2C2_BASE                       (APB1PERIPH_BASE + 0x5800)                              /* I2C2 base address */
#define I2C1_BASE                       (APB1PERIPH_BASE + 0x5400)                              /* I2C1 base address */
#define UART5_BASE                      (APB1PERIPH_BASE + 0x5000)                              /* UART5 Base Address */
#define UART4_BASE                      (APB1PERIPH_BASE + 0x4C00)                              /* UART4 Base Address */
#define USART3_BASE                     (APB1PERIPH_BASE + 0x4800)                              /* USART3 Base Address */
#define USART2_BASE                     (APB1PERIPH_BASE + 0x4400)                              /* USART2 Base Address */
#define SPI3_BASE                       (APB1PERIPH_BASE + 0x3C00)                              /* SPI3 Base Address */
#define SPI2_BASE                       (APB1PERIPH_BASE + 0x3800)                              /* SPI2 Base Address */
#define TIM14_BASE                      (APB1PERIPH_BASE + 0x2000)                              /* Timer 14 base address */
#define TIM13_BASE                      (APB1PERIPH_BASE + 0x1C00)                              /* Timer 13 base address */
#define TIM12_BASE                      (APB1PERIPH_BASE + 0x1800)                              /* Timer 12 base address */
#define TIM7_BASE                       (APB1PERIPH_BASE + 0x1400)                              /* Timer 7 base address */
#define TIM6_BASE                       (APB1PERIPH_BASE + 0x1000)                              /* Timer 6 base address */
#define TIM5_BASE                       (APB1PERIPH_BASE + 0x0C00)                              /* Timer 5 base address */
#define TIM4_BASE                       (APB1PERIPH_BASE + 0x0800)                              /* Timer 4 base address */
#define TIM3_BASE                       (APB1PERIPH_BASE + 0x0400)                              /* Timer 5 base address */
#define TIM2_BASE                       (APB1PERIPH_BASE + 0x0000)                              /* Timer 2 base address */
/* APB2 Peripherals */
#define SPI6_BASE                       (APB2PERIPH_BASE + 0x5400)                              /* SPI6 Base Address */
#define SPI5_BASE                       (APB2PERIPH_BASE + 0x5000)                              /* SPI5 Base Address */
#define TIM11_BASE                      (APB2PERIPH_BASE + 0x4800)                              /* Timer 11 base address */
#define TIM10_BASE                      (APB2PERIPH_BASE + 0x4400)                              /* Timer 10 base address */
#define TIM9_BASE                       (APB2PERIPH_BASE + 0x4000)                              /* Timer 9 base address */
#define SPI4_BASE                       (APB2PERIPH_BASE + 0x3400)                              /* SPI4 Base Address */
#define SPI1_BASE                       (APB2PERIPH_BASE + 0x3000)                              /* SPI1 Base Address */
#define USART6_BASE                     (APB2PERIPH_BASE + 0x1400)                              /* USART6 Base Address */
#define USART1_BASE                     (APB2PERIPH_BASE + 0x1000)                              /* USART1 Base Address */
#define TIM8_BASE                       (APB2PERIPH_BASE + 0x0400)                              /* Timer 8 base address */
#define TIM1_BASE                       (APB2PERIPH_BASE + 0x0000)                              /* Timer 1 base address */

/* SPI 1 is on a different bus than SPI 2/3, so we have to do a little more work to determine the correct base address */
#define SPI_BASE(port)                  (port == 1 ? (APB2PERIPH_BASE + 0x3000) : (APB1PERIPH_BASE + 0x3800 + 0x400*(port-2))) /* SPI (1,2,3) base address */

/* Power Control (PWR) */
#define PWR_CR                          (volatile uint32_t *) (PWR_BASE + 0x00)                 /* Power Control Register */
#define PWR_CSR                         (volatile uint32_t *) (PWR_BASE + 0x04)                 /* Power Control/Status Register */

/* GPIO Port (GPIO) */
#define GPIO_MODER(port)                (volatile uint32_t *) (GPIO_BASE(port) + 0x00)          /* Port mode register */
#define GPIO_OTYPER(port)               (volatile uint32_t *) (GPIO_BASE(port) + 0x04)          /* Port output type register */
#define GPIO_OSPEEDR(port)              (volatile uint32_t *) (GPIO_BASE(port) + 0x08)          /* Port output speed register */
#define GPIO_PUPDR(port)                (volatile uint32_t *) (GPIO_BASE(port) + 0x0C)          /* Port pull up/down register */
#define GPIO_IDR(port)                  (volatile uint32_t *) (GPIO_BASE(port) + 0x10)          /* Port input data register */
#define GPIO_ODR(port)                  (volatile uint32_t *) (GPIO_BASE(port) + 0x14)          /* Port output data register */
#define GPIO_BSRR(port)                 (volatile uint32_t *) (GPIO_BASE(port) + 0x18)          /* Port bit set/reset register */
#define GPIO_LCKR(port)                 (volatile uint32_t *) (GPIO_BASE(port) + 0x1C)          /* Port configuration lock register */
#define GPIO_AFRL(port)                 (volatile uint32_t *) (GPIO_BASE(port) + 0x20)          /* Port alternate function low register */
#define GPIO_AFRH(port)                 (volatile uint32_t *) (GPIO_BASE(port) + 0x24)          /* Port alternate function high register */

/* Flash Registers (FLASH) */
#define FLASH_ACR                       (volatile uint32_t *) (FLASH_R_BASE + 0x00)             /* Flash Access Control Register */

/* Direct Memory Access 1 (DMA) */
#define DMA1_LISR                       (volatile uint32_t *) (DMA1_BASE + 0x00)                /* DMA1 low interrupt status register */
#define DMA1_HISR                       (volatile uint32_t *) (DMA1_BASE + 0x04)                /* DMA1 high interrupt status register */
#define DMA1_LIFCR                      (volatile uint32_t *) (DMA1_BASE + 0x08)                /* DMA1 low interrupt flag clear register */
#define DMA1_HIFCR                      (volatile uint32_t *) (DMA1_BASE + 0x0C)                /* DMA1 high interrupt flag clear register */
/* Stream n */
#define DMA1_CR_S(n)                    (volatile uint32_t *) (DMA1_BASE + 0x10 + (0x18*n))     /* DMA1 stream n configuration register */
#define DMA1_NDTR_S(n)                  (volatile uint32_t *) (DMA1_BASE + 0x14 + (0x18*n))     /* DMA1 stream n number of data register */
#define DMA1_PAR_S(n)                   (volatile uint32_t *) (DMA1_BASE + 0x18 + (0x18*n))     /* DMA1 stream n peripheral address register */
#define DMA1_M0AR_S(n)                  (volatile uint32_t *) (DMA1_BASE + 0x1C + (0x18*n))     /* DMA1 stream n memory 0 address register */
#define DMA1_M1AR_S(n)                  (volatile uint32_t *) (DMA1_BASE + 0x20 + (0x18*n))     /* DMA1 stream n memory 1 address register */
#define DMA1_FCR_S(n)                   (volatile uint32_t *) (DMA1_BASE + 0x24 + (0x18*n))     /* DMA1 stream n FIFO control register */

/* Direct Memory Access 2 (DMA) */
#define DMA2_LISR                       (volatile uint32_t *) (DMA2_BASE + 0x00)                /* DMA2 low interrupt status register */
#define DMA2_HISR                       (volatile uint32_t *) (DMA2_BASE + 0x04)                /* DMA2 high interrupt status register */
#define DMA2_LIFCR                      (volatile uint32_t *) (DMA2_BASE + 0x08)                /* DMA2 low interrupt flag clear register */
#define DMA2_HIFCR                      (volatile uint32_t *) (DMA2_BASE + 0x0C)                /* DMA2 high interrupt flag clear register */
/* Stream n */
#define DMA2_CR_S(n)                    (volatile uint32_t *) (DMA2_BASE + 0x10 + (0x18*n))     /* DMA2 stream n configuration register */
#define DMA2_NDTR_S(n)                  (volatile uint32_t *) (DMA2_BASE + 0x14 + (0x18*n))     /* DMA2 stream n number of data register */
#define DMA2_PAR_S(n)                   (volatile uint32_t *) (DMA2_BASE + 0x18 + (0x18*n))     /* DMA2 stream n peripheral address register */
#define DMA2_M0AR_S(n)                  (volatile uint32_t *) (DMA2_BASE + 0x1C + (0x18*n))     /* DMA2 stream n memory 0 address register */
#define DMA2_M1AR_S(n)                  (volatile uint32_t *) (DMA2_BASE + 0x20 + (0x18*n))     /* DMA2 stream n memory 1 address register */
#define DMA2_FCR_S(n)                   (volatile uint32_t *) (DMA2_BASE + 0x24 + (0x18*n))     /* DMA2 stream n FIFO control register */

/* USB OTG Full-Speed */
/* Global Control and Status Registers */
#define USB_FS_GOTGCTL                  (volatile uint32_t *) (USB_FS_BASE + 0x0000)            /* USB control and status register */
#define USB_FS_GOTGINT                  (volatile uint32_t *) (USB_FS_BASE + 0x0004)            /* USB interrupt register */
#define USB_FS_GAHBCFG                  (volatile uint32_t *) (USB_FS_BASE + 0x0008)            /* USB AHB configuration register */
#define USB_FS_GUSBCFG                  (volatile uint32_t *) (USB_FS_BASE + 0x000C)            /* USB USB configuration register */
#define USB_FS_GRSTCTL                  (volatile uint32_t *) (USB_FS_BASE + 0x0010)            /* USB reset control register */
#define USB_FS_GINTSTS                  (volatile uint32_t *) (USB_FS_BASE + 0x0014)            /* USB core interrupt register */
#define USB_FS_GINTMSK                  (volatile uint32_t *) (USB_FS_BASE + 0x0018)            /* USB interrupt mask register */
#define USB_FS_GRXSTSR                  (volatile uint32_t *) (USB_FS_BASE + 0x001C)            /* USB receive status debug read register */
#define USB_FS_GRXSTSP                  (volatile uint32_t *) (USB_FS_BASE + 0x0020)            /* USB receive status debug read and pop register */
#define USB_FS_GRXFSIZ                  (volatile uint32_t *) (USB_FS_BASE + 0x0024)            /* USB receive FIFO size register */
#define USB_FS_DIEPTXF0                 (volatile uint32_t *) (USB_FS_BASE + 0x0028)            /* USB endpoint 0 transmit FIFO size register */
#define USB_FS_HNPTXFSIZ                (volatile uint32_t *) (USB_FS_BASE + 0x0028)            /* USB host non-periodic transmit FIFO size register */
#define USB_FS_HNPTXSTS                 (volatile uint32_t *) (USB_FS_BASE + 0x002C)            /* USB host non-periodic transmit FIFO/queue status register */
#define USB_FS_GCCFG                    (volatile uint32_t *) (USB_FS_BASE + 0x0038)            /* USB general core configuration register */
#define USB_FS_CID                      (volatile uint32_t *) (USB_FS_BASE + 0x003C)            /* USB core ID register */
#define USB_FS_HPTXFSIZ                 (volatile uint32_t *) (USB_FS_BASE + 0x0100)            /* USB host periodic transmit FIFO size register */
#define USB_FS_DIEPTXF1                 (volatile uint32_t *) (USB_FS_BASE + 0x0104)            /* USB device IN endpoint transmit FIFO size register 1 */
#define USB_FS_DIEPTXF2                 (volatile uint32_t *) (USB_FS_BASE + 0x0108)            /* USB device IN endpoint transmit FIFO size register 2 */
#define USB_FS_DIEPTXF3                 (volatile uint32_t *) (USB_FS_BASE + 0x010C)            /* USB device IN endpoint transmit FIFO size register 3 */

/* Host-mode Registers */
/* ... */

/* Device-mode Registers */
#define USB_FS_DCFG                     (volatile uint32_t *) (USB_FS_BASE + 0x0800)            /* USB device configuration register */
#define USB_FS_DCTL                     (volatile uint32_t *) (USB_FS_BASE + 0x0804)            /* USB device control register */
#define USB_FS_DSTS                     (volatile uint32_t *) (USB_FS_BASE + 0x0808)            /* USB device status register */
#define USB_FS_DSTS                     (volatile uint32_t *) (USB_FS_BASE + 0x0808)            /* USB device status register */
#define USB_FS_DIEPMSK                  (volatile uint32_t *) (USB_FS_BASE + 0x0810)            /* USB device IN endpoint common interrupt mask register */
#define USB_FS_DOEPMSK                  (volatile uint32_t *) (USB_FS_BASE + 0x0814)            /* USB device OUT endpoint common interrupt mask register */
#define USB_FS_DAINT                    (volatile uint32_t *) (USB_FS_BASE + 0x0818)            /* USB device all endpoints interrupt register */
#define USB_FS_DAINTMSK                 (volatile uint32_t *) (USB_FS_BASE + 0x081C)            /* USB device all endpoints interrupt mask register */
#define USB_FS_DVBUSDIS                 (volatile uint32_t *) (USB_FS_BASE + 0x0828)            /* USB device VBUS discharge time register */
#define USB_FS_DVBUSPULSE               (volatile uint32_t *) (USB_FS_BASE + 0x082C)            /* USB device VBUS pulse time register */
#define USB_FS_DIEPEMPMSK               (volatile uint32_t *) (USB_FS_BASE + 0x0834)            /* USB device IN endpoint FIFO empty interrupt mask register */
#define USB_FS_DIEPCTL0                 (volatile uint32_t *) (USB_FS_BASE + 0x0900)            /* USB device IN endpoint 0 control register */
#define USB_FS_DIEPCTL(n)               (volatile uint32_t *) (USB_FS_BASE + 0x0900 + n*0x20)   /* USB device endpoint n control register */
#define USB_FS_DIEPINT(n)               (volatile uint32_t *) (USB_FS_BASE + 0x0908 + n*0x20)   /* USB device endpoint n interrupt register */
#define USB_FS_DIEPTSIZ0                (volatile uint32_t *) (USB_FS_BASE + 0x0910)            /* USB device IN endpoint 0 transfer size register */
#define USB_FS_DIEPTSIZ(n)              (volatile uint32_t *) (USB_FS_BASE + 0x0910 + n*0x20)   /* USB device OUT endpoint n transfer size register */
#define USB_FS_DTXFSTS(n)               (volatile uint32_t *) (USB_FS_BASE + 0x0918 + n*0x20)   /* USB device IN endpoint n FIFO status register */
#define USB_FS_DOEPCTL0                 (volatile uint32_t *) (USB_FS_BASE + 0x0B00)            /* USB device OUT endpoint 0 control register */
#define USB_FS_DOEPCTL(n)               (volatile uint32_t *) (USB_FS_BASE + 0x0B00 + n*0x20)   /* USB device endpoint n control register */
#define USB_FS_DOEPINT(n)               (volatile uint32_t *) (USB_FS_BASE + 0x0B08 + n*0x20)   /* USB device endpoint 1 interrupt register */
#define USB_FS_DOEPTSIZ0                (volatile uint32_t *) (USB_FS_BASE + 0x0B10)            /* USB device OUT endpoint 0 transfer size register */
#define USB_FS_DOEPTSIZ(n)              (volatile uint32_t *) (USB_FS_BASE + 0x0B10 + n*0x20)   /* USB device OUT endpoint n transfer size register */

/* Data FIFO Registers */
#define USB_FS_DFIFO_EP(n)              (volatile uint32_t *) (USB_FS_BASE + 0x1000 + n*0x1000) /* USB endpoint n data FIFO base address */

/* Power and Clock Gating Registers */
#define USB_FS_PCGCR                    (volatile uint32_t *) (USB_FS_BASE + 0x0E00)            /* USB power and clock gating control register */


/**********************************************************************************************************************************************/

/* Bit Masks - See RM0090 Reference Manual for STM32F4 for details */
#define PWR_CR_VOS                      (uint16_t) (1 << 14)                                    /* Regulator voltage scaling output selection */

/* RCC */
#include <arch/chip/rcc.h>

#define FLASH_ACR_PRFTEN                (uint32_t) (1 << 8)                                     /* Prefetch enable */
#define FLASH_ACR_ICEN                  (uint32_t) (1 << 9)                                     /* Instruction cache enable */
#define FLASH_ACR_DCEN                  (uint32_t) (1 << 10)                                    /* Data cache enable */
#define FLASH_ACR_ICRST                 (uint32_t) (1 << 11)                                    /* Instruction cache reset */
#define FLASH_ACR_CCRST                 (uint32_t) (1 << 12)                                    /* Data cache reset */
#define FLASH_ACR_LATENCY_M             (uint32_t) (7 << 0)                                     /* Latency mask */
#define FLASH_ACR_LATENCY(n)            (uint32_t) (n << 0)                                     /* Latency - n wait states */

/* TIM */
#include <arch/chip/timer.h>

/* SPI */
#include <arch/chip/spi.h>

/* I2C */
#include <arch/chip/i2c.h>

/* USART */
#include <arch/chip/usart.h>

/* GPIO */
#define GPIOA                           (uint8_t)  (0)                                          /* GPIO Port A */
#define GPIOB                           (uint8_t)  (1)                                          /* GPIO Port B */
#define GPIOC                           (uint8_t)  (2)                                          /* GPIO Port C */
#define GPIOD                           (uint8_t)  (3)                                          /* GPIO Port D */
#define GPIOE                           (uint8_t)  (4)                                          /* GPIO Port E */
#define GPIOF                           (uint8_t)  (5)                                          /* GPIO Port F */
#define GPIOG                           (uint8_t)  (6)                                          /* GPIO Port G */
#define GPIOH                           (uint8_t)  (7)                                          /* GPIO Port H */
#define GPIOI                           (uint8_t)  (8)                                          /* GPIO Port I */

#define GPIO_MODER_PIN(n)               (uint32_t) (2*n)                                        /* Pin bitshift */
#define GPIO_MODER_M(n)                 (uint32_t) (0x3 << 2*n)                                 /* Pin mask */
#define GPIO_MODER_IN                   (uint32_t) (0x0)                                        /* Input mode */
#define GPIO_MODER_OUT                  (uint32_t) (0x1)                                        /* Output mode */
#define GPIO_MODER_ALT                  (uint32_t) (0x2)                                        /* Alternative function mode */
#define GPIO_MODER_ANA                  (uint32_t) (0x3)                                        /* Analog mode */

#define GPIO_OTYPER_PIN(n)              (uint32_t) (n)                                          /* Pin bitshift */
#define GPIO_OTYPER_M(n)                (uint32_t) (1 << n)                                     /* Pin mask */
#define GPIO_OTYPER_PP                  (uint32_t) (0x0)                                        /* Output push-pull */
#define GPIO_OTYPER_OD                  (uint32_t) (0x1)                                        /* Output open drain */

#define GPIO_OSPEEDR_PIN(n)             (uint32_t) (2*n)                                        /* Pin bitshift */
#define GPIO_OSPEEDR_M(n)               (uint32_t) (0x3 << (2*n))                               /* Pin mask */
#define GPIO_OSPEEDR_2M                 (uint32_t) (0x0)                                        /* Output speed 2MHz */
#define GPIO_OSPEEDR_25M                (uint32_t) (0x1)                                        /* Output speed 25MHz */
#define GPIO_OSPEEDR_50M                (uint32_t) (0x2)                                        /* Output speed 50MHz */
#define GPIO_OSPEEDR_100M               (uint32_t) (0x3)                                        /* Output speed 100MHz */

#define GPIO_PUPDR_PIN(n)               (uint32_t) (2*n)                                        /* Pin bitshift */
#define GPIO_PUPDR_M(n)                 (uint32_t) (0x3 << (2*n))                               /* Pin mask */
#define GPIO_PUPDR_NONE                 (uint32_t) (0x0)                                        /* Port no pull-up, pull-down */
#define GPIO_PUPDR_UP                   (uint32_t) (0x1)                                        /* Port pull-up */
#define GPIO_PUPDR_DOWN                 (uint32_t) (0x2)                                        /* Port pull-down */

#define GPIO_IDR_PIN(n)                 (uint32_t) (1 << n)                                     /* Input for pin n */

#define GPIO_ODR_PIN(n)                 (uint32_t) (1 << n)                                     /* Output for pin n */

#define GPIO_BSRR_BS(n)                 (uint32_t) (1 << n)                                     /* Set pin n */
#define GPIO_BSRR_BR(n)                 (uint32_t) (1 << (n+16))                                /* Reset pin n */

#define GPIO_AFRL_PIN(n)                (uint32_t) (4*n)                                        /* Pin bitshift */
#define GPIO_AFRL_M(n)                  (uint32_t) (0xF << (4*n))                               /* Pin mask */
#define GPIO_AFRH_PIN(n)                (uint32_t) (4*(n-8))                                    /* Pin bitshift */
#define GPIO_AFRH_M(n)                  (uint32_t) (0xF << (4*(n-8)))                           /* Pin mask */

#define GPIO_AF_USART13                 (uint32_t) (0x7)                                        /* GPIO USART1-3 mode */
#define GPIO_AF_I2C                     (uint32_t) (0x4)                                        /* GPIO I2C mode */
#define GPIO_AF_SPI12                   (uint32_t) (0x5)                                        /* GPIO SPI1-2 mode */
#define GPIO_AF_OTG                     (uint32_t) (0xA)                                        /* GPIO USB OTG mode */

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

/* USB FS */
/* Global Registers */
#define USB_FS_GOTGCTL_SRQSCS           (uint32_t) (1 << 0)                                     /* USB session request success */
#define USB_FS_GOTGCTL_SRQ              (uint32_t) (1 << 1)                                     /* USB session request */
#define USB_FS_GOTGCTL_HNGSCS           (uint32_t) (1 << 8)                                     /* USB host negotiation success */
#define USB_FS_GOTGCTL_HNP              (uint32_t) (1 << 9)                                     /* USB HNP request */
#define USB_FS_GOTGCTL_HSHNPEN          (uint32_t) (1 << 10)                                    /* USB host set HPN enable */
#define USB_FS_GOTGCTL_DHNPEN           (uint32_t) (1 << 11)                                    /* USB device HPN enabled */
#define USB_FS_GOTGCTL_CIDSTS           (uint32_t) (1 << 16)                                    /* USB connector ID status */
#define USB_FS_GOTGCTL_DBCT             (uint32_t) (1 << 17)                                    /* USB debounce time */
#define USB_FS_GOTGCTL_ASVLD            (uint32_t) (1 << 18)                                    /* USB A session valid */
#define USB_FS_GOTGCTL_BSVLD            (uint32_t) (1 << 19)                                    /* USB B session valid */

#define USB_FS_GOTGINT_SEDET            (uint32_t) (1 << 2)                                     /* USB session end detected */
#define USB_FS_GOTGINT_SRSSCHG          (uint32_t) (1 << 8)                                     /* USB session request success status change */
#define USB_FS_GOTGINT_HNSSCHG          (uint32_t) (1 << 9)                                     /* USB host negotiation success status change */
#define USB_FS_GOTGINT_HNGDET           (uint32_t) (1 << 17)                                    /* USB host negotiation detected */
#define USB_FS_GOTGINT_ADTOCHG          (uint32_t) (1 << 18)                                    /* USB A-device timeout change */
#define USB_FS_GOTGINT_DBCDNE           (uint32_t) (1 << 19)                                    /* USB debounce done */

#define USB_FS_GAHBCFG_GINTMSK          (uint32_t) (1 << 0)                                     /* USB global interrupt mask */
#define USB_FS_GAHBCFG_TXFELVL          (uint32_t) (1 << 7)                                     /* USB TX FIFO empty level */
#define USB_FS_GAHBCFG_PTXFELVL         (uint32_t) (1 << 8)                                     /* USB Periodic TX FIFO empty level */

#define USB_FS_GUSBCFG_TOCAL(n)         (uint32_t) (n << 0)                                     /* USB FS timeout calibration */
#define USB_FS_GUSBCFG_PHYSEL           (uint32_t) (1 << 7)                                     /* USB FS serial transceiver select */
#define USB_FS_GUSBCFG_SRPCAP           (uint32_t) (1 << 8)                                     /* USB SRP capable */
#define USB_FS_GUSBCFG_HNPCAP           (uint32_t) (1 << 9)                                     /* USB HNP capable */
#define USB_FS_GUSBCFG_TRDT(n)          (uint32_t) (n << 10)                                    /* USB turnaround time (4 bits) */
#define USB_FS_GUSBCFG_FHMOD            (uint32_t) (1 << 29)                                    /* USB force host mode */
#define USB_FS_GUSBCFG_FDMOD            (uint32_t) (1 << 30)                                    /* USB force device mode */
#define USB_FS_GUSBCFG_CTXPKT           (uint32_t) (1 << 31)                                    /* USB corrupt packet */

#define USB_FS_GRSTCTL_CSRST            (uint32_t) (1 << 0)                                     /* USB core soft reset */
#define USB_FS_GRSTCTL_HSRST            (uint32_t) (1 << 1)                                     /* USB HCLK soft reset */
#define USB_FS_GRSTCTL_FCRST            (uint32_t) (1 << 2)                                     /* USB host frame counter reset */
#define USB_FS_GRSTCTL_RXFFLSH          (uint32_t) (1 << 4)                                     /* USB RX FIFO flush */
#define USB_FS_GRSTCTL_TXFFLSH          (uint32_t) (1 << 5)                                     /* USB TX FIFO flush */
#define USB_FS_GRSTCTL_TXFNUM(n)        (uint32_t) (n << 6)                                     /* USB TX FIFO number */
#define USB_FS_GRSTCTL_AHBIDL           (uint32_t) (1 << 31)                                    /* USB AHB master idle */

#define USB_FS_GINTSTS_CMOD             (uint32_t) (1 << 0)                                     /* USB current mode of operation */
#define USB_FS_GINTSTS_MMIS             (uint32_t) (1 << 1)                                     /* USB mode mismatch interrupt */
#define USB_FS_GINTSTS_OTGINT           (uint32_t) (1 << 2)                                     /* USB OTG interrupt */
#define USB_FS_GINTSTS_SOF              (uint32_t) (1 << 3)                                     /* USB start of frame */
#define USB_FS_GINTSTS_RXFLVL           (uint32_t) (1 << 4)                                     /* USB RX FIFO non-empty */
#define USB_FS_GINTSTS_NPTXFE           (uint32_t) (1 << 5)                                     /* USB non-periodic TX FIFO empty */
#define USB_FS_GINTSTS_GINAKEFF         (uint32_t) (1 << 6)                                     /* USB global IN non-periodic NAK effective */
#define USB_FS_GINTSTS_GONAKEFF         (uint32_t) (1 << 7)                                     /* USB global OUT NAK effective */
#define USB_FS_GINTSTS_ESUSP            (uint32_t) (1 << 10)                                    /* USB early suspend */
#define USB_FS_GINTSTS_USBSUSP          (uint32_t) (1 << 11)                                    /* USB suspend */
#define USB_FS_GINTSTS_USBRST           (uint32_t) (1 << 12)                                    /* USB reset */
#define USB_FS_GINTSTS_ENUMDNE          (uint32_t) (1 << 13)                                    /* USB enumeration done */
#define USB_FS_GINTSTS_ISOODRP          (uint32_t) (1 << 14)                                    /* USB isochronous OUT packet dropped interrupt */
#define USB_FS_GINTSTS_EOPF             (uint32_t) (1 << 15)                                    /* USB end of packet frame interrupt */
#define USB_FS_GINTSTS_IEPINT           (uint32_t) (1 << 18)                                    /* USB IN endpoint interrupt */
#define USB_FS_GINTSTS_OEPINT           (uint32_t) (1 << 19)                                    /* USB OUT endpoint interrupt */
#define USB_FS_GINTSTS_IISOIXFR         (uint32_t) (1 << 20)                                    /* USB incomplete isochronous IN transfer */
#define USB_FS_GINTSTS_IISOOXFR         (uint32_t) (1 << 21)                                    /* USB incomplete isochronous OUT transfer */
#define USB_FS_GINTSTS_IPXFR            (uint32_t) (1 << 21)                                    /* USB incomplete periodic transfer */
#define USB_FS_GINTSTS_HPRTINT          (uint32_t) (1 << 24)                                    /* USB host port interrupt */
#define USB_FS_GINTSTS_HCINT            (uint32_t) (1 << 25)                                    /* USB host channels interrupt */
#define USB_FS_GINTSTS_PTXFE            (uint32_t) (1 << 26)                                    /* USB periodic TX FIFO empty */
#define USB_FS_GINTSTS_CIDSCHG          (uint32_t) (1 << 28)                                    /* USB connector ID status change */
#define USB_FS_GINTSTS_DISCINT          (uint32_t) (1 << 29)                                    /* USB disconnect detected interrupt */
#define USB_FS_GINTSTS_SRQINT           (uint32_t) (1 << 30)                                    /* USB session request/new session detected interrupt */
#define USB_FS_GINTSTS_WKUPINT          (uint32_t) (1 << 31)                                    /* USB resume/remote wakeup detected interrupt */

#define USB_FS_GINTMSK_MMISM            (uint32_t) (1 << 1)                                     /* USB mode mismatch interrupt mask */
#define USB_FS_GINTMSK_OTGINT           (uint32_t) (1 << 2)                                     /* USB OTG interrupt mask */
#define USB_FS_GINTMSK_SOFM             (uint32_t) (1 << 3)                                     /* USB start of frame mask */
#define USB_FS_GINTMSK_RXFLVLM          (uint32_t) (1 << 4)                                     /* USB RX FIFO non-empty mask */
#define USB_FS_GINTMSK_NPTXFEM          (uint32_t) (1 << 5)                                     /* USB non-periodic TX FIFO empty mask */
#define USB_FS_GINTMSK_GINAKEFFM        (uint32_t) (1 << 6)                                     /* USB global IN non-periodic NAK effective mask */
#define USB_FS_GINTMSK_GONAKEFFM        (uint32_t) (1 << 7)                                     /* USB global OUT NAK effective mask */
#define USB_FS_GINTMSK_ESUSPM           (uint32_t) (1 << 10)                                    /* USB early suspend mask */
#define USB_FS_GINTMSK_USBSUSPM         (uint32_t) (1 << 11)                                    /* USB suspend mask */
#define USB_FS_GINTMSK_USBRSTM          (uint32_t) (1 << 12)                                    /* USB reset mask */
#define USB_FS_GINTMSK_ENUMDNEM         (uint32_t) (1 << 13)                                    /* USB enumeration done mask */
#define USB_FS_GINTMSK_ISOODRPM         (uint32_t) (1 << 14)                                    /* USB isochronous OUT packet dropped interrupt mask */
#define USB_FS_GINTMSK_EOPFM            (uint32_t) (1 << 15)                                    /* USB end of packet frame interrupt mask */
#define USB_FS_GINTMSK_EPMISM           (uint32_t) (1 << 17)                                    /* USB endpoint mismatch interrupt mask */
#define USB_FS_GINTMSK_IEPINT           (uint32_t) (1 << 18)                                    /* USB IN endpoint interrupt mask */
#define USB_FS_GINTMSK_OEPINT           (uint32_t) (1 << 19)                                    /* USB OUT endpoint interrupt mask */
#define USB_FS_GINTMSK_IISOIXFRM        (uint32_t) (1 << 20)                                    /* USB incomplete isochronous IN transfer mask */
#define USB_FS_GINTMSK_IISOOXFRM        (uint32_t) (1 << 21)                                    /* USB incomplete isochronous OUT transfer mask */
#define USB_FS_GINTMSK_IPXFRM           (uint32_t) (1 << 21)                                    /* USB incomplete periodic transfer mask */
#define USB_FS_GINTMSK_HPRTINT          (uint32_t) (1 << 24)                                    /* USB host port interrupt mask */
#define USB_FS_GINTMSK_HCINT            (uint32_t) (1 << 25)                                    /* USB host channels interrupt mask */
#define USB_FS_GINTMSK_PTXFEM           (uint32_t) (1 << 26)                                    /* USB periodic TX FIFO empty mask */
#define USB_FS_GINTMSK_CIDSCHGM         (uint32_t) (1 << 28)                                    /* USB connector ID status change mask */
#define USB_FS_GINTMSK_DISCINT          (uint32_t) (1 << 29)                                    /* USB disconnect detected interrupt mask */
#define USB_FS_GINTMSK_SRQINT           (uint32_t) (1 << 30)                                    /* USB session request/new session detected interrupt mask */
#define USB_FS_GINTMSK_WKUPINT          (uint32_t) (1 << 31)                                    /* USB resume/remote wakeup detected interrupt mask */

#define USB_FS_GRXSTS_EPNUM(r)          (uint32_t) (r & 0xF)                                    /* USB RX FIFO endpoint number */
#define USB_FS_GRXSTS_BCNT(r)           (uint32_t) ((r & 0x7FF0) >> 4)                          /* USB RX FIFO byte count */
#define USB_FS_GRXSTS_DPID(r)           (uint32_t) ((r & 0x18000) >> 15)                        /* USB RX FIFO data PID */
#define USB_FS_GRXSTS_PKTSTS(r)         (uint32_t) ((r & 0x1E0000) >> 17)                       /* USB RX FIFO packet status */
#define USB_FS_GRXSTS_FRMNUM(r)         (uint32_t) ((r & 0x1E00000) >> 21)                      /* USB RX FIFO frame number */
#define USB_FS_GRXSTS_PKTSTS_NAK        (uint8_t)  (1)                                          /* USB RX FIFO packet status = global OUT NAK */
#define USB_FS_GRXSTS_PKTSTS_ORX        (uint8_t)  (2)                                          /* USB RX FIFO packet status = OUT data packet received */
#define USB_FS_GRXSTS_PKTSTS_OCP        (uint8_t)  (3)                                          /* USB RX FIFO packet status = OUT transfer completed */
#define USB_FS_GRXSTS_PKTSTS_STUPCP     (uint8_t)  (4)                                          /* USB RX FIFO packet status = SETUP transaction completed */
#define USB_FS_GRXSTS_PKTSTS_STUPRX     (uint8_t)  (6)                                          /* USB RX FIFO packet status = SETUP data packet received */

#define USB_FS_DIEPTXF0_TX0FSA(n)       (uint32_t) (n << 0)                                     /* USB endpoint 0 transmit RAM start address */
#define USB_FS_DIEPTXF0_TX0FD(n)        (uint32_t) (n << 16)                                    /* USB endpoint 0 TX FIFO depth */

#define USB_FS_GCCFG_PWRDWN             (uint32_t) (1 << 16)                                    /* USB power down */
#define USB_FS_GCCFG_VBUSASEN           (uint32_t) (1 << 18)                                    /* USB VBUS "A" sensing enable */
#define USB_FS_GCCFG_VBUSBSEN           (uint32_t) (1 << 19)                                    /* USB VBUS "B" sensing enable */
#define USB_FS_GCCFG_SOFOUTEN           (uint32_t) (1 << 20)                                    /* USB SOF output enable */
#define USB_FS_GCCFG_NOVBUSSSENS        (uint32_t) (1 << 21)                                    /* USB VBUS sensing disable */

#define USB_FS_DIEPTXF_INEPTXSA(n)      (uint32_t) (n << 0)                                     /* USB IN endpoint FIFOx transmit RAM start address */
#define USB_FS_DIEPTXF_INEPTXFD(n)      (uint32_t) (n << 16)                                    /* USB IN endpoint TX FIFOx depth */

/* Device-mode Registers */
#define USB_FS_DCFG_DSPD_FS             (uint32_t) (1 << 1 | 1 << 0)                            /* USB device speed: full speed (USB 1.1) */
#define USB_FS_DCFG_NZLSOHSK            (uint32_t) (1 << 2)                                     /* USB device non-zero-length status OUT handshake */
#define USB_FS_DCFG_DAD(x)              (uint32_t) (x << 4)                                     /* USB device address */
#define USB_FS_DCFG_PFIVL_80            (uint32_t) (0 << 11)                                    /* USB device periodic frame interval: 80% */
#define USB_FS_DCFG_PFIVL_85            (uint32_t) (1 << 11)                                    /* USB device periodic frame interval: 85% */
#define USB_FS_DCFG_PFIVL_90            (uint32_t) (2 << 11)                                    /* USB device periodic frame interval: 90% */
#define USB_FS_DCFG_PFIVL_95            (uint32_t) (3 << 11)                                    /* USB device periodic frame interval: 95% */

#define USB_FS_DCTL_RWUSIG              (uint32_t) (1 << 0)                                     /* USB device remote wakeup signaling */
#define USB_FS_DCTL_SDIS                (uint32_t) (1 << 1)                                     /* USB device soft disconnect */
#define USB_FS_DCTL_GINSTS              (uint32_t) (1 << 2)                                     /* USB device global IN NAK status */
#define USB_FS_DCTL_GONSTS              (uint32_t) (1 << 3)                                     /* USB device global OUT NAK status */
#define USB_FS_DCTL_TCTL_J              (uint32_t) (1 << 4)                                     /* USB device Test_J mode */
#define USB_FS_DCTL_TCTL_K              (uint32_t) (2 << 4)                                     /* USB device Test_K mode */
#define USB_FS_DCTL_TCTL_SE0_NAK        (uint32_t) (3 << 4)                                     /* USB device Test_SE0_NAK mode */
#define USB_FS_DCTL_TCTL_PKT            (uint32_t) (4 << 4)                                     /* USB device Test_Packet mode */
#define USB_FS_DCTL_TCTL_FEN            (uint32_t) (5 << 4)                                     /* USB device Test_Force_Enable mode */
#define USB_FS_DCTL_SGINAK              (uint32_t) (1 << 7)                                     /* USB device set global IN NAK */
#define USB_FS_DCTL_CGINAK              (uint32_t) (1 << 8)                                     /* USB device clear global IN NAK */
#define USB_FS_DCTL_SGONAK              (uint32_t) (1 << 9)                                     /* USB device set global OUT NAK */
#define USB_FS_DCTL_CGONAK              (uint32_t) (1 << 10)                                    /* USB device clear global OUT NAK */
#define USB_FS_DCTL_POPRGDNE            (uint32_t) (1 << 11)                                    /* USB device power-no programming done */

#define USB_FS_DSTS_SUSPSTS             (uint32_t) (1 << 0)                                     /* USB device suspend status */
#define USB_FS_DSTS_ENUMSPD             (uint32_t) (3 << 1)                                     /* USB device enumerated speed (Must == 3) */
#define USB_FS_DSTS_ENUMSPD_FS          (uint32_t) (3 << 1)                                          /* USB device enumerated speed (Must == 3) */
#define USB_FS_DSTS_EERR                (uint32_t) (1 << 3)                                     /* USB device erratic error */
#define USB_FS_DSTS_FNSOF               (uint32_t) (0x3FFF00)                                   /* USB device frame number of received SOF (bits 21:8) */

#define USB_FS_DIEPMSK_XFRCM            (uint32_t) (1 << 0)                                     /* USB device transfer completed interrupt mask */
#define USB_FS_DIEPMSK_EPDM             (uint32_t) (1 << 1)                                     /* USB device endpoint disabled interrupt mask */
#define USB_FS_DIEPMSK_TOM              (uint32_t) (1 << 3)                                     /* USB device timout condition mask */
#define USB_FS_DIEPMSK_ITTXFEMSK        (uint32_t) (1 << 4)                                     /* USB device IN token received when TX FIFO empty mask */
#define USB_FS_DIEPMSK_INEPNMM          (uint32_t) (1 << 5)                                     /* USB device IN token received with EP mismatch mask */
#define USB_FS_DIEPMSK_INEPNEM          (uint32_t) (1 << 6)                                     /* USB device IN endpoint NAK effective mask */

#define USB_FS_DOEPMSK_XFRCM            (uint32_t) (1 << 0)                                     /* USB device transfer completed interrupt mask */
#define USB_FS_DOEPMSK_EPDM             (uint32_t) (1 << 1)                                     /* USB device endpoint disabled interrupt mask */
#define USB_FS_DOEPMSK_STUPM            (uint32_t) (1 << 3)                                     /* USB device SETUP phase done mask */
#define USB_FS_DOEPMSK_OTEPDM           (uint32_t) (1 << 4)                                     /* USB device OUT token received when endpoint disabled mask */

#define USB_FS_DAINT_IEPINT(n)          (uint32_t) (1 << n)                                     /* USB device IN endpoint interrupt bits */
#define USB_FS_DAINT_OEPINT(n)          (uint32_t) (1 << (n+16))                                /* USB device OUT endpoint interrupt bits */

#define USB_FS_DAINT_IEPM(n)            (uint32_t) (1 << n)                                     /* USB device IN endpoint interrupt mask bits */
#define USB_FS_DAINT_OEPM(n)            (uint32_t) (1 << (n+16))                                /* USB device OUT endpoint interrupt mask bits */

#define USB_FS_DIEPEMPMSK_INEPTXFEM(n)  (uint32_t) (1 << n)                                     /* USB device IN EP TX FIFO empty interrupt mask bits */

#define USB_FS_DIEPCTL0_MPSIZE          (uint32_t) (3 << 0)                                     /* USB device endpoint 0 IN maximum packet size */
#define USB_FS_DIEPCTL0_MPSIZE_64       (uint32_t) (0 << 0)                                     /* USB device endpoint 0 IN maximum packet size 64 bytes */
#define USB_FS_DIEPCTL0_MPSIZE_32       (uint32_t) (1 << 0)                                     /* USB device endpoint 0 IN maximum packet size 32 bytes */
#define USB_FS_DIEPCTL0_MPSIZE_16       (uint32_t) (2 << 0)                                     /* USB device endpoint 0 IN maximum packet size 16 bytes */
#define USB_FS_DIEPCTL0_MPSIZE_8        (uint32_t) (3 << 0)                                     /* USB device endpoint 0 IN maximum packet size 8 bytes */
#define USB_FS_DIEPCTL0_USBAEP          (uint32_t) (1 << 15)                                    /* USB device endpoint 0 IN USB active endpoint (always 1) */
#define USB_FS_DIEPCTL0_NAKSTS          (uint32_t) (1 << 17)                                    /* USB device endpoint 0 IN NAK status */
#define USB_FS_DIEPCTL0_STALL           (uint32_t) (1 << 21)                                    /* USB device endpoint 0 IN STALL handshake */
#define USB_FS_DIEPCTL0_TXFNUM(n)       (uint32_t) (n << 22)                                    /* USB device endpoint 0 IN TX FIFO number */
#define USB_FS_DIEPCTL0_CNAK            (uint32_t) (1 << 26)                                    /* USB device endpoint 0 IN clear NAK */
#define USB_FS_DIEPCTL0_SNAK            (uint32_t) (1 << 27)                                    /* USB device endpoint 0 IN set NAK */
#define USB_FS_DIEPCTL0_EPDIS           (uint32_t) (1 << 30)                                    /* USB device endpoint 0 IN endpoint disable */
#define USB_FS_DIEPCTL0_EPENA           (uint32_t) (1 << 31)                                    /* USB device endpoint 0 IN endpoint enable */

#define USB_FS_DIEPCTLx_MPSIZE(n)       (uint32_t) (n << 0)                                     /* USB device endpoint x IN maximum packet size in bytes (11 bits) */
#define USB_FS_DIEPCTLx_USBAEP          (uint32_t) (1 << 15)                                    /* USB device endpoint x IN USB active endpoint */
#define USB_FS_DIEPCTLx_EONUM_EVEN      (uint32_t) (0 << 16)                                    /* USB device endpoint x IN Odd frame */
#define USB_FS_DIEPCTLx_EONUM_ODD       (uint32_t) (1 << 16)                                    /* USB device endpoint x IN Odd frame */
#define USB_FS_DIEPCTLx_DPID_0          (uint32_t) (0 << 16)                                    /* USB device endpoint x IN data PID 0 */
#define USB_FS_DIEPCTLx_DPID_1          (uint32_t) (1 << 16)                                    /* USB device endpoint x IN data PID 1 */
#define USB_FS_DIEPCTLx_NAKSTS          (uint32_t) (1 << 17)                                    /* USB device endpoint x IN NAK status */
#define USB_FS_DIEPCTLx_EPTYP_CTL       (uint32_t) (0 << 18)                                    /* USB device endpoint x IN type: control */
#define USB_FS_DIEPCTLx_EPTYP_ISO       (uint32_t) (1 << 18)                                    /* USB device endpoint x IN type: isochronous */
#define USB_FS_DIEPCTLx_EPTYP_BLK       (uint32_t) (2 << 18)                                    /* USB device endpoint x IN type: bulk */
#define USB_FS_DIEPCTLx_EPTYP_INT       (uint32_t) (3 << 18)                                    /* USB device endpoint x IN type: interrupt */
#define USB_FS_DIEPCTLx_STALL           (uint32_t) (1 << 21)                                    /* USB device endpoint x IN STALL handshake */
#define USB_FS_DIEPCTLx_TXFNUM(n)       (uint32_t) (n << 22)                                    /* USB device endpoint x IN TX FIFO number (4 bits) */
#define USB_FS_DIEPCTLx_CNAK            (uint32_t) (1 << 26)                                    /* USB device endpoint x IN clear NAK */
#define USB_FS_DIEPCTLx_SNAK            (uint32_t) (1 << 27)                                    /* USB device endpoint x IN set NAK */
#define USB_FS_DIEPCTLx_SD0PID          (uint32_t) (1 << 28)                                    /* USB device endpoint x IN set DATA0 PID */
#define USB_FS_DIEPCTLx_SEVNFRM         (uint32_t) (1 << 28)                                    /* USB device endpoint x IN set even frame */
#define USB_FS_DIEPCTLx_SODDFRM         (uint32_t) (1 << 29)                                    /* USB device endpoint x IN set odd frame */
#define USB_FS_DIEPCTLx_EPDIS           (uint32_t) (1 << 30)                                    /* USB device endpoint x IN endpoint disable */
#define USB_FS_DIEPCTLx_EPENA           (uint32_t) (1 << 31)                                    /* USB device endpoint x IN endpoint enable */

#define USB_FS_DOEPCTL0_MPSIZE          (uint32_t) (3 << 0)                                     /* USB device endpoint 0 OUT maximum packet size */
#define USB_FS_DOEPCTL0_MPSIZE_64       (uint32_t) (0 << 0)                                     /* USB device endpoint 0 OUT maximum packet size 64 bytes */
#define USB_FS_DOEPCTL0_MPSIZE_32       (uint32_t) (1 << 0)                                     /* USB device endpoint 0 OUT maximum packet size 32 bytes */
#define USB_FS_DOEPCTL0_MPSIZE_16       (uint32_t) (2 << 0)                                     /* USB device endpoint 0 OUT maximum packet size 16 bytes */
#define USB_FS_DOEPCTL0_MPSIZE_8        (uint32_t) (3 << 0)                                     /* USB device endpoint 0 OUT maximum packet size 8 bytes */
#define USB_FS_DOEPCTL0_USBAEP          (uint32_t) (1 << 15)                                    /* USB device endpoint 0 OUT USB active endpoint (always 1) */
#define USB_FS_DOEPCTL0_NAKSTS          (uint32_t) (1 << 17)                                    /* USB device endpoint 0 OUT NAK status */
#define USB_FS_DOEPCTL0_SNPM            (uint32_t) (1 << 20)                                    /* USB device endpoint 0 OUT snoop mode */
#define USB_FS_DOEPCTL0_STALL           (uint32_t) (1 << 21)                                    /* USB device endpoint 0 OUT STALL handshake */
#define USB_FS_DOEPCTL0_CNAK            (uint32_t) (1 << 26)                                    /* USB device endpoint 0 OUT clear NAK */
#define USB_FS_DOEPCTL0_SNAK            (uint32_t) (1 << 27)                                    /* USB device endpoint 0 OUT set NAK */
#define USB_FS_DOEPCTL0_EPDIS           (uint32_t) (1 << 30)                                    /* USB device endpoint 0 OUT endpoint disable */
#define USB_FS_DOEPCTL0_EPENA           (uint32_t) (1 << 31)                                    /* USB device endpoint 0 OUT endpoint enable */

#define USB_FS_DOEPCTLx_MPSIZE(n)       (uint32_t) (n << 0)                                     /* USB device endpoint x OUT maximum packet size in bytes (11 bits) */
#define USB_FS_DOEPCTLx_USBAEP          (uint32_t) (1 << 15)                                    /* USB device endpoint x OUT USB active endpoint */
#define USB_FS_DOEPCTLx_EONUM_EVEN      (uint32_t) (0 << 16)                                    /* USB device endpoint x OUT Odd frame */
#define USB_FS_DOEPCTLx_EONUM_ODD       (uint32_t) (1 << 16)                                    /* USB device endpoint x OUT Odd frame */
#define USB_FS_DOEPCTLx_DPID_0          (uint32_t) (0 << 16)                                    /* USB device endpoint x OUT data PID 0 */
#define USB_FS_DOEPCTLx_DPID_1          (uint32_t) (1 << 16)                                    /* USB device endpoint x OUT data PID 1 */
#define USB_FS_DOEPCTLx_NAKSTS          (uint32_t) (1 << 17)                                    /* USB device endpoint x OUT NAK status */
#define USB_FS_DOEPCTLx_EPTYP_CTL       (uint32_t) (0 << 18)                                    /* USB device endpoint x OUT type: control */
#define USB_FS_DOEPCTLx_EPTYP_ISO       (uint32_t) (1 << 18)                                    /* USB device endpoint x OUT type: isochronous */
#define USB_FS_DOEPCTLx_EPTYP_BLK       (uint32_t) (2 << 18)                                    /* USB device endpoint x OUT type: bulk */
#define USB_FS_DOEPCTLx_EPTYP_INT       (uint32_t) (3 << 18)                                    /* USB device endpoint x OUT type: interrupt */
#define USB_FS_DOEPCTLx_SNPMN           (uint32_t) (1 << 20)                                    /* USB device endpoint x OUT snoop mode */
#define USB_FS_DOEPCTLx_STALL           (uint32_t) (1 << 21)                                    /* USB device endpoint x OUT STALL handshake */
#define USB_FS_DOEPCTLx_CNAK            (uint32_t) (1 << 26)                                    /* USB device endpoint x OUT clear NAK */
#define USB_FS_DOEPCTLx_SNAK            (uint32_t) (1 << 27)                                    /* USB device endpoint x OUT set NAK */
#define USB_FS_DOEPCTLx_SD0PID          (uint32_t) (1 << 28)                                    /* USB device endpoint x OUT set DATA0 PID */
#define USB_FS_DOEPCTLx_SEVNFRM         (uint32_t) (1 << 28)                                    /* USB device endpoint x OUT set even frame */
#define USB_FS_DOEPCTLx_SODDFRM         (uint32_t) (1 << 29)                                    /* USB device endpoint x OUT set odd frame */
#define USB_FS_DOEPCTLx_EPDIS           (uint32_t) (1 << 30)                                    /* USB device endpoint x OUT endpoint disable */
#define USB_FS_DOEPCTLx_EPENA           (uint32_t) (1 << 31)                                    /* USB device endpoint x OUT endpoint enable */

#define USB_FS_DIEPINTx_XFRC            (uint32_t) (1 << 0)                                     /* USB device endpoint x IN transfer complete interrupt */
#define USB_FS_DIEPINTx_EPDISD          (uint32_t) (1 << 1)                                     /* USB device endpoint x IN endpoint disabled interrupt */
#define USB_FS_DIEPINTx_TOC             (uint32_t) (1 << 3)                                     /* USB device endpoint x IN timeout condition */
#define USB_FS_DIEPINTx_ITTXFE          (uint32_t) (1 << 4)                                     /* USB device endpoint x IN token received when TX FIFO is empty */
#define USB_FS_DIEPINTx_INEPNE          (uint32_t) (1 << 6)                                     /* USB device endpoint x IN endpoint NAK effective */
#define USB_FS_DIEPINTx_TXFE            (uint32_t) (1 << 7)                                     /* USB device endpoint x IN TX FIFO empty */

#define USB_FS_DOEPINTx_XFRC            (uint32_t) (1 << 0)                                     /* USB device endpoint x OUT transfer complete interrupt */
#define USB_FS_DOEPINTx_EPDISD          (uint32_t) (1 << 1)                                     /* USB device endpoint x OUT endpoint disabled interrupt */
#define USB_FS_DOEPINTx_STUP            (uint32_t) (1 << 3)                                     /* USB device endpoint x OUT SETUP phase complete */
#define USB_FS_DOEPINTx_OTEPDIS         (uint32_t) (1 << 4)                                     /* USB device endpoint x OUT token receieved when endpoint disabled */
#define USB_FS_DOEPINTx_B2BSTUP         (uint32_t) (1 << 6)                                     /* USB device endpoint x OUT back-to-back SETUP packets received */

#define USB_FS_DIEPTSIZ0_XFRSIZ(n)      (uint32_t) (n << 0)                                     /* USB device endpoint 0 IN transfer size (7 bits) */
#define USB_FS_DIEPTSIZ0_PKTCNT(n)      (uint32_t) (n << 19)                                    /* USB device endpoint 0 IN packet count */

#define USB_FS_DIEPTSIZx_XFRSIZ(n)      (uint32_t) (n << 0)                                     /* USB device endpoint x IN transfer size (19 bits) */
#define USB_FS_DIEPTSIZx_PKTCNT(n)      (uint32_t) (n << 19)                                    /* USB device endpoint x IN packet count (10 bits) */
#define USB_FS_DIEPTSIZx_MCNT_1         (uint32_t) (1 << 29)                                    /* USB device endpoint x IN multi count 1 */
#define USB_FS_DIEPTSIZx_MCNT_2         (uint32_t) (2 << 29)                                    /* USB device endpoint x IN multi count 1 */
#define USB_FS_DIEPTSIZx_MCNT_3         (uint32_t) (3 << 29)                                    /* USB device endpoint x IN multi count 1 */

#define USB_FS_DOEPTSIZ0_XFRSIZ(n)      (uint32_t) (n << 0)                                     /* USB device endpoint 0 OUT transfer size (7 bits) */
#define USB_FS_DOEPTSIZ0_PKTCNT(n)      (uint32_t) (n << 19)                                    /* USB device endpoint 0 OUT packet count */
#define USB_FS_DOEPTSIZ0_STUPCNT(n)     (uint32_t) (n << 29)                                    /* USB device endpoint 0 OUT back-to-back SETUP packets allowed */

#define USB_FS_DOEPTSIZx_XFRSIZ(n)      (uint32_t) (n << 0)                                     /* USB device endpoint x OUT transfer size (19 bits) */
#define USB_FS_DOEPTSIZx_PKTCNT(n)      (uint32_t) (n << 19)                                    /* USB device endpoint x OUT packet count (10 bits) */
#define USB_FS_DOEPTSIZx_STUPCNT(n)     (uint32_t) (n << 29)                                    /* USB device endpoint x OUT back-to-back SETUP packets allowed */
#define USB_FS_DOEPTSIZx_RXDPID         (uint32_t) (1 << 29 | 1 << 30)                          /* USB device endpoint x OUT received data PID */

/* Powe and Clock Gating Control Register */
#define USB_FS_PCGCCTL_STPPCLK          (uint32_t) (1 << 0)                                     /* USB stop PHY clock */
#define USB_FS_PCGCCTL_GATEHCLK         (uint32_t) (1 << 1)                                     /* USB gate HCLK */
#define USB_FS_PCGCCTL_PHYSUSP          (uint32_t) (1 << 4)                                     /* USB PHY suspended */

#endif
