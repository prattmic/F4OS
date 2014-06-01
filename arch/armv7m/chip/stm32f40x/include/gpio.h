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

#ifndef ARCH_CHIP_GPIO_H_INCLUDED
#define ARCH_CHIP_GPIO_H_INCLUDED

#include <stdint.h>
#include <atomic.h>
#include <arch/chip/periph.h>
#include <arch/chip/registers.h>

inline static void gpio_moder(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg;

    do {
        reg = load_link(GPIO_MODER(port));

        reg &= ~(GPIO_MODER_M(pin));
        reg |= (mode << GPIO_MODER_PIN(pin));
    } while (store_conditional(GPIO_MODER(port), reg));
}

inline static void gpio_otyper(uint8_t port, uint8_t pin, uint8_t type) {
    uint32_t reg;

    do {
        reg = load_link(GPIO_OTYPER(port));

        reg &= ~(GPIO_OTYPER_M(pin));
        reg |= (type << GPIO_OTYPER_PIN(pin));
    } while(store_conditional(GPIO_OTYPER(port), reg));
}

inline static void gpio_ospeedr(uint8_t port, uint8_t pin, uint8_t speed) {
    uint32_t reg;

    do {
        reg = load_link(GPIO_OSPEEDR(port));

        reg &= ~(GPIO_OSPEEDR_M(pin));
        reg |= (speed << GPIO_OSPEEDR_PIN(pin));
    } while (store_conditional(GPIO_OSPEEDR(port), reg));
}

inline static void gpio_pupdr(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg;

    do {
        reg = load_link(GPIO_PUPDR(port));

        reg &= ~(GPIO_PUPDR_M(pin));
        reg |= (mode << GPIO_PUPDR_PIN(pin));
    } while (store_conditional(GPIO_PUPDR(port), reg));
}

inline static void gpio_afr(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg;

    if (pin < 8) {
        do {
            reg = load_link(GPIO_AFRL(port));
            reg &= ~(GPIO_AFRL_M(pin));
            reg |= (mode << GPIO_AFRL_PIN(pin));
        } while(store_conditional(GPIO_AFRL(port), reg));
    }
    else {
        do {
            reg = load_link(GPIO_AFRH(port));
            reg &= ~(GPIO_AFRH_M(pin));
            reg |= (mode << GPIO_AFRH_PIN(pin));
        } while (store_conditional(GPIO_AFRH(port), reg));
    }
}

/* Additional flags for direction */
enum stm32f4_direction_gpio_flags {
    STM32F4_GPIO_DIRECTION_ANALOG = (1 << 16),  /* Set GPIO to analog I/O */
    STM32F4_GPIO_DIRECTION_MASK = (1 << 16),
};

/* Valid flags for set_flags */
enum stm32f4_gpio_flags {
    STM32F4_GPIO_ALT_FUNC,
    STM32F4_GPIO_PULL,
    STM32F4_GPIO_SPEED,
    STM32F4_GPIO_TYPE,
    STM32F4_GPIO_CLOCK_ENABLE,
};

/* GPIO alternative functions */
enum stm32f4_gpio_alt_funcs {
    STM32F4_GPIO_AF_SYSTEM      = 0,
    STM32F4_GPIO_AF_TIM1        = 1,
    STM32F4_GPIO_AF_TIM2        = 1,
    STM32F4_GPIO_AF_TIM3        = 2,
    STM32F4_GPIO_AF_TIM4        = 2,
    STM32F4_GPIO_AF_TIM5        = 2,
    STM32F4_GPIO_AF_TIM8        = 3,
    STM32F4_GPIO_AF_TIM9        = 3,
    STM32F4_GPIO_AF_TIM10       = 3,
    STM32F4_GPIO_AF_TIM11       = 3,
    STM32F4_GPIO_AF_I2C         = 4,
    STM32F4_GPIO_AF_SPI1        = 5,
    STM32F4_GPIO_AF_SPI2        = 5,
    STM32F4_GPIO_AF_SPI3        = 6,
    STM32F4_GPIO_AF_USART1      = 7,
    STM32F4_GPIO_AF_USART2      = 7,
    STM32F4_GPIO_AF_USART3      = 7,
    STM32F4_GPIO_AF_USART4      = 8,
    STM32F4_GPIO_AF_USART5      = 8,
    STM32F4_GPIO_AF_USART6      = 8,
    STM32F4_GPIO_AF_CAN         = 9,
    STM32F4_GPIO_AF_TIM12       = 9,
    STM32F4_GPIO_AF_TIM13       = 9,
    STM32F4_GPIO_AF_TIM14       = 9,
    STM32F4_GPIO_AF_OTG_FS      = 10,
    STM32F4_GPIO_AF_OTG_HS      = 10,
    STM32F4_GPIO_AF_ETH         = 11,
    STM32F4_GPIO_AF_FSMC        = 12,
    STM32F4_GPIO_AF_SDIO        = 12,
    STM32F4_GPIO_AF_OTG_HS_FS   = 12,   /* USB OTG HS configured as FS */
    STM32F4_GPIO_AF_DCMI        = 13,
    STM32F4_GPIO_AF_EVENTOUT    = 15,
    STM32F4_GPIO_AF_UNKNOWN     = -1,
};

enum stm32f4_gpio_pull {
    STM32F4_GPIO_PULL_NONE  = GPIO_PUPDR_NONE,
    STM32F4_GPIO_PULL_UP    = GPIO_PUPDR_UP,
    STM32F4_GPIO_PULL_DOWN  = GPIO_PUPDR_DOWN,
};

enum stm32f4_gpio_speed {
    STM32F4_GPIO_SPEED_2MHZ     = GPIO_OSPEEDR_2M,
    STM32F4_GPIO_SPEED_25MHZ    = GPIO_OSPEEDR_25M,
    STM32F4_GPIO_SPEED_50MHZ    = GPIO_OSPEEDR_50M,
    STM32F4_GPIO_SPEED_100MHZ   = GPIO_OSPEEDR_100M,
};

enum stm32f4_gpio_type {
    STM32F4_GPIO_PUSH_PULL  = GPIO_OTYPER_PP,
    STM32F4_GPIO_OPEN_DRAIN = GPIO_OTYPER_OD,
};

/* Enumerate all of the GPIOs */
enum stm32f4_gpios {
    STM32F4_GPIO_PA0 = 0,
    STM32F4_GPIO_PA1,
    STM32F4_GPIO_PA2,
    STM32F4_GPIO_PA3,
    STM32F4_GPIO_PA4,
    STM32F4_GPIO_PA5,
    STM32F4_GPIO_PA6,
    STM32F4_GPIO_PA7,
    STM32F4_GPIO_PA8,
    STM32F4_GPIO_PA9,
    STM32F4_GPIO_PA10,
    STM32F4_GPIO_PA11,
    STM32F4_GPIO_PA12,
    STM32F4_GPIO_PA13,
    STM32F4_GPIO_PA14,
    STM32F4_GPIO_PA15,
    STM32F4_GPIO_PB0,
    STM32F4_GPIO_PB1,
    STM32F4_GPIO_PB2,
    STM32F4_GPIO_PB3,
    STM32F4_GPIO_PB4,
    STM32F4_GPIO_PB5,
    STM32F4_GPIO_PB6,
    STM32F4_GPIO_PB7,
    STM32F4_GPIO_PB8,
    STM32F4_GPIO_PB9,
    STM32F4_GPIO_PB10,
    STM32F4_GPIO_PB11,
    STM32F4_GPIO_PB12,
    STM32F4_GPIO_PB13,
    STM32F4_GPIO_PB14,
    STM32F4_GPIO_PB15,
    STM32F4_GPIO_PC0,
    STM32F4_GPIO_PC1,
    STM32F4_GPIO_PC2,
    STM32F4_GPIO_PC3,
    STM32F4_GPIO_PC4,
    STM32F4_GPIO_PC5,
    STM32F4_GPIO_PC6,
    STM32F4_GPIO_PC7,
    STM32F4_GPIO_PC8,
    STM32F4_GPIO_PC9,
    STM32F4_GPIO_PC10,
    STM32F4_GPIO_PC11,
    STM32F4_GPIO_PC12,
    STM32F4_GPIO_PC13,
    STM32F4_GPIO_PC14,
    STM32F4_GPIO_PC15,
    STM32F4_GPIO_PD0,
    STM32F4_GPIO_PD1,
    STM32F4_GPIO_PD2,
    STM32F4_GPIO_PD3,
    STM32F4_GPIO_PD4,
    STM32F4_GPIO_PD5,
    STM32F4_GPIO_PD6,
    STM32F4_GPIO_PD7,
    STM32F4_GPIO_PD8,
    STM32F4_GPIO_PD9,
    STM32F4_GPIO_PD10,
    STM32F4_GPIO_PD11,
    STM32F4_GPIO_PD12,
    STM32F4_GPIO_PD13,
    STM32F4_GPIO_PD14,
    STM32F4_GPIO_PD15,
    STM32F4_GPIO_PE0,
    STM32F4_GPIO_PE1,
    STM32F4_GPIO_PE2,
    STM32F4_GPIO_PE3,
    STM32F4_GPIO_PE4,
    STM32F4_GPIO_PE5,
    STM32F4_GPIO_PE6,
    STM32F4_GPIO_PE7,
    STM32F4_GPIO_PE8,
    STM32F4_GPIO_PE9,
    STM32F4_GPIO_PE10,
    STM32F4_GPIO_PE11,
    STM32F4_GPIO_PE12,
    STM32F4_GPIO_PE13,
    STM32F4_GPIO_PE14,
    STM32F4_GPIO_PE15,
    STM32F4_GPIO_PF0,
    STM32F4_GPIO_PF1,
    STM32F4_GPIO_PF2,
    STM32F4_GPIO_PF3,
    STM32F4_GPIO_PF4,
    STM32F4_GPIO_PF5,
    STM32F4_GPIO_PF6,
    STM32F4_GPIO_PF7,
    STM32F4_GPIO_PF8,
    STM32F4_GPIO_PF9,
    STM32F4_GPIO_PF10,
    STM32F4_GPIO_PF11,
    STM32F4_GPIO_PF12,
    STM32F4_GPIO_PF13,
    STM32F4_GPIO_PF14,
    STM32F4_GPIO_PF15,
    STM32F4_GPIO_PG0,
    STM32F4_GPIO_PG1,
    STM32F4_GPIO_PG2,
    STM32F4_GPIO_PG3,
    STM32F4_GPIO_PG4,
    STM32F4_GPIO_PG5,
    STM32F4_GPIO_PG6,
    STM32F4_GPIO_PG7,
    STM32F4_GPIO_PG8,
    STM32F4_GPIO_PG9,
    STM32F4_GPIO_PG10,
    STM32F4_GPIO_PG11,
    STM32F4_GPIO_PG12,
    STM32F4_GPIO_PG13,
    STM32F4_GPIO_PG14,
    STM32F4_GPIO_PG15,
    STM32F4_GPIO_PH0,
    STM32F4_GPIO_PH1,
    STM32F4_GPIO_PH2,
    STM32F4_GPIO_PH3,
    STM32F4_GPIO_PH4,
    STM32F4_GPIO_PH5,
    STM32F4_GPIO_PH6,
    STM32F4_GPIO_PH7,
    STM32F4_GPIO_PH8,
    STM32F4_GPIO_PH9,
    STM32F4_GPIO_PH10,
    STM32F4_GPIO_PH11,
    STM32F4_GPIO_PH12,
    STM32F4_GPIO_PH13,
    STM32F4_GPIO_PH14,
    STM32F4_GPIO_PH15,
    STM32F4_GPIO_PI0,
    STM32F4_GPIO_PI1,
    STM32F4_GPIO_PI2,
    STM32F4_GPIO_PI3,
    STM32F4_GPIO_PI4,
    STM32F4_GPIO_PI5,
    STM32F4_GPIO_PI6,
    STM32F4_GPIO_PI7,
    STM32F4_GPIO_PI8,
    STM32F4_GPIO_PI9,
    STM32F4_GPIO_PI10,
    STM32F4_GPIO_PI11,
    STM32F4_NUM_GPIOS,
};

/*
 * Lookup AF number from periph id
 *
 * Use the STM32F4 peripheral id to determine the appropriate GPIO AF
 * register entry.
 *
 * @param id    Periph ID to look up
 * @return AF number, or STM32F4_GPIO_AF_UNKNOWN if not found/known
 */
enum stm32f4_gpio_alt_funcs gpio_periph_to_alt_func(enum stm32f4_periph_id id);

#endif
