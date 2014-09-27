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

#include <stdint.h>
#include <arch/chip/rcc.h>
#include <arch/chip/periph.h>
#include <dev/raw_mem.h>

struct rcc_clock_bits {
    volatile uint32_t *reg;
    uint32_t bits;
};

static const struct rcc_clock_bits rcc_clock_lookup[NUM_STM32F4_PERIPH] = {
    [STM32F4_PERIPH_GPIOA] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOAEN },
    [STM32F4_PERIPH_GPIOB] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOBEN },
    [STM32F4_PERIPH_GPIOC] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOCEN },
    [STM32F4_PERIPH_GPIOD] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIODEN },
    [STM32F4_PERIPH_GPIOE] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOEEN },
    [STM32F4_PERIPH_GPIOF] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOFEN },
    [STM32F4_PERIPH_GPIOG] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOGEN },
    [STM32F4_PERIPH_GPIOH] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOHEN },
    [STM32F4_PERIPH_GPIOI] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_GPIOIEN },
    [STM32F4_PERIPH_TIM1] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_TIM1EN },
    [STM32F4_PERIPH_TIM2] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM2EN },
    [STM32F4_PERIPH_TIM3] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM3EN },
    [STM32F4_PERIPH_TIM4] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM4EN },
    [STM32F4_PERIPH_TIM5] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM5EN },
    [STM32F4_PERIPH_TIM6] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM6EN },
    [STM32F4_PERIPH_TIM7] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM7EN },
    [STM32F4_PERIPH_TIM8] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_TIM8EN },
    [STM32F4_PERIPH_TIM9] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_TIM9EN },
    [STM32F4_PERIPH_TIM10] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_TIM10EN },
    [STM32F4_PERIPH_TIM11] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_TIM11EN },
    [STM32F4_PERIPH_TIM12] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM12EN },
    [STM32F4_PERIPH_TIM13] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM13EN },
    [STM32F4_PERIPH_TIM14] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_TIM14EN },
    [STM32F4_PERIPH_SPI1] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SPI1EN },
    [STM32F4_PERIPH_SPI2] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_SPI2EN },
    [STM32F4_PERIPH_SPI3] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_SPI3EN },
    [STM32F4_PERIPH_SPI4] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SPI4EN },
    [STM32F4_PERIPH_SPI5] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SPI5EN },
    [STM32F4_PERIPH_SPI6] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SPI6EN },
    [STM32F4_PERIPH_I2C1] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_I2C1EN },
    [STM32F4_PERIPH_I2C2] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_I2C2EN },
    [STM32F4_PERIPH_I2C3] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_I2C3EN },
    [STM32F4_PERIPH_USART1] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_USART1EN },
    [STM32F4_PERIPH_USART2] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_USART2EN },
    [STM32F4_PERIPH_USART3] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_USART3EN },
    [STM32F4_PERIPH_USART4] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_USART4EN },
    [STM32F4_PERIPH_USART5] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_USART5EN },
    [STM32F4_PERIPH_USART6] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_USART6EN },
    [STM32F4_PERIPH_CAN1] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_CAN1EN },
    [STM32F4_PERIPH_CAN2] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_CAN2EN },
    [STM32F4_PERIPH_SDIO] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SDIOEN },
    [STM32F4_PERIPH_ETH] = { .reg = RCC_AHB1ENR,
        .bits = RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN |
            RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACPTPEN},
    [STM32F4_PERIPH_DCMI] = { .reg = RCC_AHB2ENR, .bits = RCC_AHB2ENR_DCMIEN },
    [STM32F4_PERIPH_FSMC] = { .reg = RCC_AHB3ENR, .bits = RCC_AHB3ENR_FSMCEN },
    [STM32F4_PERIPH_DMA1] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_DMA1EN },
    [STM32F4_PERIPH_DMA2] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_DMA2EN },
    [STM32F4_PERIPH_ADC1] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_ADC1EN },
    [STM32F4_PERIPH_ADC2] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_ADC2EN },
    [STM32F4_PERIPH_ADC3] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_ADC3EN },
    [STM32F4_PERIPH_DAC] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_DACEN },
    [STM32F4_PERIPH_USBFS] = { .reg = RCC_AHB2ENR, .bits = RCC_AHB2ENR_OTGFSEN },
    [STM32F4_PERIPH_USBHS] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_OTGHSEN },
    [STM32F4_PERIPH_WWDG] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_WWDGEN },
    [STM32F4_PERIPH_PWR] = { .reg = RCC_APB1ENR, .bits = RCC_APB1ENR_PWREN },
    [STM32F4_PERIPH_SYSCFG] = { .reg = RCC_APB2ENR, .bits = RCC_APB2ENR_SYSCFGEN },
    [STM32F4_PERIPH_CRC] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_CRCEN },
    [STM32F4_PERIPH_RNG] = { .reg = RCC_AHB2ENR, .bits = RCC_AHB2ENR_RNGEN },
    [STM32F4_PERIPH_HASH] = { .reg = RCC_AHB2ENR, .bits = RCC_AHB2ENR_HASHEN },
    [STM32F4_PERIPH_CRYP] = { .reg = RCC_AHB2ENR, .bits = RCC_AHB2ENR_CRYPEN },
    [STM32F4_PERIPH_CCMRAM] = { .reg = RCC_AHB1ENR, .bits = RCC_AHB1ENR_CCMDATARAMEN },
};

int rcc_set_clock_enable(enum stm32f4_periph_id periphid, int enable) {
    struct rcc_clock_bits periph;

    if (periphid >= NUM_STM32F4_PERIPH) {
        /* Unknown peripheral! */
        return -1;
    }

    periph = rcc_clock_lookup[periphid];

    if (enable) {
        raw_mem_set_bits(periph.reg, periph.bits);
    }
    else {
        raw_mem_clear_bits(periph.reg, periph.bits);
    }

    return 0;
}

static const enum stm32f4_bus periph_bus_lookup[NUM_STM32F4_PERIPH] = {
    [STM32F4_PERIPH_GPIOA]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOB]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOC]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOD]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOE]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOF]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOG]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOH]  = STM32F4_AHB1,
    [STM32F4_PERIPH_GPIOI]  = STM32F4_AHB1,
    [STM32F4_PERIPH_TIM1]   = STM32F4_APB2,
    [STM32F4_PERIPH_TIM2]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM3]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM4]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM5]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM6]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM7]   = STM32F4_APB1,
    [STM32F4_PERIPH_TIM8]   = STM32F4_APB2,
    [STM32F4_PERIPH_TIM9]   = STM32F4_APB2,
    [STM32F4_PERIPH_TIM10]  = STM32F4_APB2,
    [STM32F4_PERIPH_TIM11]  = STM32F4_APB2,
    [STM32F4_PERIPH_TIM12]  = STM32F4_APB1,
    [STM32F4_PERIPH_TIM13]  = STM32F4_APB1,
    [STM32F4_PERIPH_TIM14]  = STM32F4_APB1,
    [STM32F4_PERIPH_SPI1]   = STM32F4_APB2,
    [STM32F4_PERIPH_SPI2]   = STM32F4_APB2,
    [STM32F4_PERIPH_SPI3]   = STM32F4_APB2,
    [STM32F4_PERIPH_SPI4]   = STM32F4_APB2,
    [STM32F4_PERIPH_SPI5]   = STM32F4_APB2,
    [STM32F4_PERIPH_SPI6]   = STM32F4_APB2,
    [STM32F4_PERIPH_I2C1]   = STM32F4_APB1,
    [STM32F4_PERIPH_I2C2]   = STM32F4_APB1,
    [STM32F4_PERIPH_I2C3]   = STM32F4_APB1,
    [STM32F4_PERIPH_USART1] = STM32F4_APB2,
    [STM32F4_PERIPH_USART2] = STM32F4_APB1,
    [STM32F4_PERIPH_USART3] = STM32F4_APB1,
    [STM32F4_PERIPH_USART4] = STM32F4_APB1,
    [STM32F4_PERIPH_USART5] = STM32F4_APB1,
    [STM32F4_PERIPH_USART6] = STM32F4_APB2,
    [STM32F4_PERIPH_CAN1]   = STM32F4_APB1,
    [STM32F4_PERIPH_CAN2]   = STM32F4_APB1,
    [STM32F4_PERIPH_SDIO]   = STM32F4_APB2,
    [STM32F4_PERIPH_ETH]    = STM32F4_AHB1,
    [STM32F4_PERIPH_DCMI]   = STM32F4_AHB2,
    [STM32F4_PERIPH_FSMC]   = STM32F4_AHB3,
    [STM32F4_PERIPH_DMA1]   = STM32F4_AHB1,
    [STM32F4_PERIPH_DMA2]   = STM32F4_AHB1,
    [STM32F4_PERIPH_ADC1]   = STM32F4_APB2,
    [STM32F4_PERIPH_ADC2]   = STM32F4_APB2,
    [STM32F4_PERIPH_ADC3]   = STM32F4_APB2,
    [STM32F4_PERIPH_DAC]    = STM32F4_APB1,
    [STM32F4_PERIPH_USBFS]  = STM32F4_AHB2,
    [STM32F4_PERIPH_USBHS]  = STM32F4_AHB1,
    [STM32F4_PERIPH_WWDG]   = STM32F4_APB1,
    [STM32F4_PERIPH_PWR]    = STM32F4_APB1,
    [STM32F4_PERIPH_SYSCFG] = STM32F4_APB2,
    [STM32F4_PERIPH_CRC]    = STM32F4_AHB1,
    [STM32F4_PERIPH_RNG]    = STM32F4_AHB2,
    [STM32F4_PERIPH_HASH]   = STM32F4_AHB2,
    [STM32F4_PERIPH_CRYP]   = STM32F4_AHB2,
    [STM32F4_PERIPH_CCMRAM] = STM32F4_AHB1,
};

enum stm32f4_bus rcc_peripheral_bus(enum stm32f4_periph_id periphid) {
    if (periphid >= NUM_STM32F4_PERIPH) {
        /* Unknown peripheral! */
        return STM32F4_UNKNOWN_BUS;
    }

    return periph_bus_lookup[periphid];
}

long rcc_bus_clock(enum stm32f4_bus bus) {
    switch (bus) {
    case STM32F4_AHB1:
    case STM32F4_AHB2:
    case STM32F4_AHB3:
        return CONFIG_SYS_CLOCK;
    case STM32F4_APB1:
        return CONFIG_SYS_CLOCK / 4;
    case STM32F4_APB2:
        return CONFIG_SYS_CLOCK / 2;
    default: /* Unknown! */
        return -1;
    }
}
