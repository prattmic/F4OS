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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/chip/gpio.h>
#include <arch/chip/periph.h>
#include <arch/chip/registers.h>
#include <dev/hw/gpio.h>
#include <kernel/class.h>

static const enum stm32f4_gpio_alt_funcs gpio_alt_funcs_lookup[NUM_STM32F4_PERIPH] = {
    [STM32F4_PERIPH_GPIOA] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOB] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOC] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOD] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOE] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOF] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOG] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOH] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_GPIOI] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_TIM1] = STM32F4_GPIO_AF_TIM1,
    [STM32F4_PERIPH_TIM2] = STM32F4_GPIO_AF_TIM2,
    [STM32F4_PERIPH_TIM3] = STM32F4_GPIO_AF_TIM3,
    [STM32F4_PERIPH_TIM4] = STM32F4_GPIO_AF_TIM4,
    [STM32F4_PERIPH_TIM5] = STM32F4_GPIO_AF_TIM5,
    [STM32F4_PERIPH_TIM6] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_TIM7] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_TIM8] = STM32F4_GPIO_AF_TIM8,
    [STM32F4_PERIPH_TIM9] = STM32F4_GPIO_AF_TIM9,
    [STM32F4_PERIPH_TIM10] = STM32F4_GPIO_AF_TIM10,
    [STM32F4_PERIPH_TIM11] = STM32F4_GPIO_AF_TIM11,
    [STM32F4_PERIPH_TIM12] = STM32F4_GPIO_AF_TIM12,
    [STM32F4_PERIPH_TIM13] = STM32F4_GPIO_AF_TIM13,
    [STM32F4_PERIPH_TIM14] = STM32F4_GPIO_AF_TIM14,
    [STM32F4_PERIPH_SPI1] = STM32F4_GPIO_AF_SPI1,
    [STM32F4_PERIPH_SPI2] = STM32F4_GPIO_AF_SPI2,
    [STM32F4_PERIPH_SPI3] = STM32F4_GPIO_AF_SPI3,
    [STM32F4_PERIPH_SPI4] = STM32F4_GPIO_AF_SPI4,
    [STM32F4_PERIPH_SPI5] = STM32F4_GPIO_AF_SPI5,
    [STM32F4_PERIPH_SPI6] = STM32F4_GPIO_AF_SPI6,
    [STM32F4_PERIPH_I2C1] = STM32F4_GPIO_AF_I2C,
    [STM32F4_PERIPH_I2C2] = STM32F4_GPIO_AF_I2C,
    [STM32F4_PERIPH_I2C3] = STM32F4_GPIO_AF_I2C,
    [STM32F4_PERIPH_USART1] = STM32F4_GPIO_AF_USART1,
    [STM32F4_PERIPH_USART2] = STM32F4_GPIO_AF_USART2,
    [STM32F4_PERIPH_USART3] = STM32F4_GPIO_AF_USART3,
    [STM32F4_PERIPH_USART4] = STM32F4_GPIO_AF_USART4,
    [STM32F4_PERIPH_USART5] = STM32F4_GPIO_AF_USART5,
    [STM32F4_PERIPH_USART6] = STM32F4_GPIO_AF_USART6,
    [STM32F4_PERIPH_CAN1] = STM32F4_GPIO_AF_CAN,
    [STM32F4_PERIPH_CAN2] = STM32F4_GPIO_AF_CAN,
    [STM32F4_PERIPH_SDIO] = STM32F4_GPIO_AF_SDIO,
    [STM32F4_PERIPH_ETH] = STM32F4_GPIO_AF_ETH,
    [STM32F4_PERIPH_DCMI] = STM32F4_GPIO_AF_DCMI,
    [STM32F4_PERIPH_FSMC] = STM32F4_GPIO_AF_FSMC,
    [STM32F4_PERIPH_DMA1] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_DMA2] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_ADC1] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_ADC2] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_ADC3] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_DAC] = STM32F4_GPIO_AF_SYSTEM,
    [STM32F4_PERIPH_USBFS] = STM32F4_GPIO_AF_OTG_FS,
    [STM32F4_PERIPH_USBHS] = STM32F4_GPIO_AF_OTG_HS,
    [STM32F4_PERIPH_WWDG] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_PWR] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_SYSCFG] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_CRC] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_RNG] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_HASH] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_CRYP] = STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
    [STM32F4_PERIPH_CCMRAM] =  STM32F4_GPIO_AF_UNKNOWN, /* No GPIO I/O */
};

enum stm32f4_gpio_alt_funcs gpio_periph_to_alt_func(enum stm32f4_periph_id id) {
    if (id >= NUM_STM32F4_PERIPH) {
        return -1;
    }

    return gpio_alt_funcs_lookup[id];
}

/* Describe GPIOs in a more convienient way for us */
struct stm32f4_gpio {
    uint8_t port;   /* GPIO port number.  Port A == 0, Port I == 8 */
    uint8_t pin;    /* GPIO pin number on port.  0..15 */
};

static struct stm32f4_gpio stm32f4_gpio_decode(uint32_t num) {
    struct stm32f4_gpio gpio;

    gpio.port = num/16;
    gpio.pin = num % 16;

    return gpio;
}

/*
 * Set to standard reset values:
 * input, push-pull, low speed, no pull up/down
 */
static int stm32f4_gpio_reset(struct gpio *g) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);

    gpio_moder(gpio.port, gpio.pin, GPIO_MODER_IN);
    gpio_otyper(gpio.port, gpio.pin, GPIO_OTYPER_PP);
    gpio_ospeedr(gpio.port, gpio.pin, GPIO_OSPEEDR_2M);
    gpio_pupdr(gpio.port, gpio.pin, GPIO_PUPDR_NONE);

    g->active_low = 0;

    return 0;
}

static int stm32f4_gpio_active_low(struct gpio *g, int active_low) {
    /* Force active_low to a binary value */
    g->active_low = !!active_low;

    return 0;
}

/**
 * Additionally supports flags in enum stm32f4_direction_gpio_flags:
 * STM32F4_GPIO_DIRECTION_ANALOG:
 *  Sets GPIO to analog mode.  Standard GPIO_INPUT/GPIO_OUTPUT flags
 *  are ignored, as this mode is used for both analog input and output.
 */
static int stm32f4_gpio_direction(struct gpio *g, int flags) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);
    int mode = 0;
    int ret = 0;

    /* Analog mode does not depend on standard direction flags */
    switch (flags & STM32F4_GPIO_DIRECTION_MASK) {
    case STM32F4_GPIO_DIRECTION_ANALOG:
        mode = GPIO_MODER_ANA;
        break;
    }

    if (!mode) {
        switch (flags & GPIO_DIRECTION_MASK) {
        case GPIO_INPUT:
            mode = GPIO_MODER_IN;
            break;
        case GPIO_OUTPUT:
            mode = GPIO_MODER_OUT;
            break;
        default:
            ret = -1;
            goto out;
        }
    }

    gpio_moder(gpio.port, gpio.pin, mode);

out:
    return ret;
}

static unsigned int stm32f4_gpio_get_input_value(struct gpio *g) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);

    uint32_t reg = *GPIO_IDR(gpio.port);

    /* Flip value if active low */
    return ((reg >> gpio.pin) & 1) ^ g->active_low;
}

static int stm32f4_gpio_set_output_value(struct gpio *g, unsigned int val) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);

    /* Force val to 0 or 1, flip if active low */
    val = !!val ^ g->active_low;

    /*
     * Set or reset output bit.
     * Set bits start at 0, reset bits start at 16
     */
    *GPIO_BSRR(gpio.port) = 1 << (gpio.pin + 16*(!val));

    return 0;
}

static unsigned int stm32f4_gpio_get_output_value(struct gpio *g) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);

    uint32_t reg = *GPIO_ODR(gpio.port);

    /* Flip value if active low */
    return ((reg >> gpio.pin) & 1) ^ g->active_low;
}

/*
 * Enable the approriate RCC clock for gpio.
 */
static inline void stm32f4_gpio_clock_enable(struct stm32f4_gpio gpio) {
    uint32_t reg;

    do {
        reg = load_link(RCC_AHB1ENR);
        reg |= 1 << gpio.port;
    } while (store_conditional(RCC_AHB1ENR, reg));
}

/*
 * Valid flags:
 * STM32F4_GPIO_ALT_FUNC:
 *  set GPIO to alternative function val
 *  @param val  Alternative function from enum stm32f4_gpio_alt_funcs
 *  @returns zero on success, negative on error
 * STM32F4_GPIO_PULL:
 *  set GPIO pull up/down
 *  @param val  Pull up/down setting from enum stm32f4_gpio_pull
 *  @returns zero on success, negative on error
 * STM32F4_GPIO_SPEED:
 *  set GPIO output speed
 *  @param val  Output speed setting from enum stm32f4_gpio_speed
 *  @returns zero on success, negative on error
 * STM32F4_GPIO_TYPE:
 *  set GPIO output type
 *  @param val  Output type setting from enum stm32f4_gpio_type
 *  @returns zero on success, negative on error
 * STM32F4_GPIO_CLOCK_ENABLE:
 *  Enables the RCC clock controling the GPIO port.
 *  @param val  Don't care
 *  @returns zero on success, negative on error
 */
static int stm32f4_gpio_set_flags(struct gpio *g, unsigned int flag, int val) {
    struct stm32f4_gpio gpio = stm32f4_gpio_decode(g->num);

    switch (flag) {
    case STM32F4_GPIO_ALT_FUNC:
        gpio_moder(gpio.port, gpio.pin, GPIO_MODER_ALT);
        gpio_afr(gpio.port, gpio.pin, val);
        return 0;
    case STM32F4_GPIO_PULL:
        gpio_pupdr(gpio.port, gpio.pin, val);
        return 0;
    case STM32F4_GPIO_SPEED:
        gpio_ospeedr(gpio.port, gpio.pin, val);
        return 0;
    case STM32F4_GPIO_TYPE:
        gpio_otyper(gpio.port, gpio.pin, val);
        return 0;
    case STM32F4_GPIO_CLOCK_ENABLE:
        stm32f4_gpio_clock_enable(gpio);
        return 0;
    default:
        return GPIO_ERR_FLG_UNSUPPT;
    }
}

static int stm32f4_gpio_dtor(struct gpio *gpio) {
    /* Nothing to do */

    return 0;
}

static struct gpio_ops stm32f4_gpio_ops = {
    .reset = stm32f4_gpio_reset,
    .active_low = stm32f4_gpio_active_low,
    .direction = stm32f4_gpio_direction,
    .get_input_value = stm32f4_gpio_get_input_value,
    .set_output_value = stm32f4_gpio_set_output_value,
    .get_output_value = stm32f4_gpio_get_output_value,
    .set_flags = stm32f4_gpio_set_flags,
    .dtor = stm32f4_gpio_dtor,
};

/*
 * Build name of GPIO in buf.
 * buf must be at least 5 bytes long.
 * Name is in form "PB12"
 */
static void stm32f4_gpio_build_name(struct stm32f4_gpio gpio, char *buf) {
    buf[0] = 'P';
    buf[1] = 'A' + gpio.port;   /* Ports start at A */

    /* Set pin number.  Always <16, fits in 3 characters */
    itoa(gpio.pin, &buf[2], 3, 10);
}

/* Implement exported interface defined in dev/hw/gpio.h */
int gpio_valid(uint32_t gpio) {
    if (gpio < STM32F4_NUM_GPIOS) {
        return 0;
    }

    return GPIO_ERR_INVAL;
}

struct obj *_gpio_instantiate(uint32_t gpio) {
    struct stm32f4_gpio g = stm32f4_gpio_decode(gpio);

    char name[5] = {'\0'};
    stm32f4_gpio_build_name(g, name);

    struct obj *o = instantiate(name, &gpio_class, &stm32f4_gpio_ops, struct gpio);
    if (!o) {
        return NULL;
    }

    stm32f4_gpio_clock_enable(g);

    /* Export to the OS */
    class_export_member(o);

    return o;
}
