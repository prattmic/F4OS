#ifndef ARCH_CHIP_GPIO_H_INCLUDED
#define ARCH_CHIP_GPIO_H_INCLUDED

#include <arch/atomic.h>
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

#endif
