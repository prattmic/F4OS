#ifndef ARCH_CHIP_GPIO_H_INCLUDED
#define ARCH_CHIP_GPIO_H_INCLUDED

#include <arch/chip/registers.h>

inline static void gpio_moder(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg = *GPIO_MODER(port);

    reg &= ~(GPIO_MODER_M(pin));
    reg |= (mode << GPIO_MODER_PIN(pin));

    *GPIO_MODER(port) = reg;
}

inline static void gpio_otyper(uint8_t port, uint8_t pin, uint8_t type) {
    uint32_t reg = *GPIO_OTYPER(port);

    reg &= ~(GPIO_OTYPER_M(pin));
    reg |= (type << GPIO_OTYPER_PIN(pin));

    *GPIO_OTYPER(port) = reg;
}

inline static void gpio_ospeedr(uint8_t port, uint8_t pin, uint8_t speed) {
    uint32_t reg = *GPIO_OSPEEDR(port);

    reg &= ~(GPIO_OSPEEDR_M(pin));
    reg |= (speed << GPIO_OSPEEDR_PIN(pin));

    *GPIO_OSPEEDR(port) = reg;
}

inline static  void gpio_pupdr(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg = *GPIO_PUPDR(port);

    reg &= ~(GPIO_PUPDR_M(pin));
    reg |= (mode << GPIO_PUPDR_PIN(pin));

    *GPIO_PUPDR(port) = reg;
}

inline static void gpio_afr(uint8_t port, uint8_t pin, uint8_t mode) {
    uint32_t reg;

    if (pin < 8) {
        reg = *GPIO_AFRL(port);
        reg &= ~(GPIO_AFRL_M(pin));
        reg |= (mode << GPIO_AFRL_PIN(pin));
        *GPIO_AFRL(port) = reg;
    }
    else {
        reg = *GPIO_AFRH(port);
        reg &= ~(GPIO_AFRH_M(pin));
        reg |= (mode << GPIO_AFRH_PIN(pin));
        *GPIO_AFRH(port) = reg;
    }
}

#endif
