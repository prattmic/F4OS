#ifndef ARCH_CHIP_GPIO_H_INCLUDED
#define ARCH_CHIP_GPIO_H_INCLUDED

#include <stdint.h>
#include <atomic.h>
#include <arch/chip/registers.h>

/* Describe GPIOs in a more convienient way for us */
struct lm4f_gpio {
    uint8_t port;   /* GPIO port number */
    uint8_t pin;    /* GPIO pin number on port.  0..8 */
};

enum lm4f_gpio_ports {
    LM4F_GPIO_PORTA,
    LM4F_GPIO_PORTB,
    LM4F_GPIO_PORTC,
    LM4F_GPIO_PORTD,
    LM4F_GPIO_PORTE,
    LM4F_GPIO_PORTF,
};

struct lm4f_gpio_regs {
    uint32_t DATA;  /* Bits [9:2] of address used as mask */
    uint32_t reserved1[0x3fc/4];
    uint32_t DIR;
    uint32_t IS;
    uint32_t IBE;
    uint32_t IEV;
    uint32_t IM;
    uint32_t RIS;
    uint32_t MIS;
    uint32_t ICR;
    uint32_t AFSEL;
    uint32_t reserved2[0xdc/4];
    uint32_t DR2R;
    uint32_t DR4R;
    uint32_t DR8R;
    uint32_t DR;
    uint32_t PUR;
    uint32_t PDR;
    uint32_t SLR;
    uint32_t DEN;
    uint32_t LOCK;
    uint32_t CR;
    uint32_t AMSEL;
    uint32_t PCTL;
    uint32_t ADCTL;
    uint32_t DMACTL;
    uint32_t reserved3[0xa98/4];
    uint32_t PERIPHID4;
    uint32_t PERIPHID5;
    uint32_t PERIPHID6;
    uint32_t PERIPHID7;
    uint32_t PERIPHID0;
    uint32_t PERIPHID1;
    uint32_t PERIPHID2;
    uint32_t PERIPHID3;
    uint32_t PCELLD0;
    uint32_t PCELLD1;
    uint32_t PCELLD2;
    uint32_t PCELLD3;
};

static inline struct lm4f_gpio_regs *gpio_get_regs(int port) {
    switch (port) {
        case LM4F_GPIO_PORTA: return (struct lm4f_gpio_regs *) GPIO_PORTA_AHB_BASE;
        case LM4F_GPIO_PORTB: return (struct lm4f_gpio_regs *) GPIO_PORTB_AHB_BASE;
        case LM4F_GPIO_PORTC: return (struct lm4f_gpio_regs *) GPIO_PORTC_AHB_BASE;
        case LM4F_GPIO_PORTD: return (struct lm4f_gpio_regs *) GPIO_PORTD_AHB_BASE;
        case LM4F_GPIO_PORTE: return (struct lm4f_gpio_regs *) GPIO_PORTE_AHB_BASE;
        case LM4F_GPIO_PORTF: return (struct lm4f_gpio_regs *) GPIO_PORTF_AHB_BASE;
    }
    return (struct lm4f_gpio_regs *) INVALID_PERIPH_BASE;
}

inline static void lm4f_gpio_dir(struct lm4f_gpio *gpio, uint8_t dir) {
    struct lm4f_gpio_regs *regs = gpio_get_regs(gpio->port);
    uint32_t val;

    do {
        val = load_link(&regs->DIR);

        val &= ~(1 << gpio->pin);
        val |= (dir << gpio->pin);
    } while (store_conditional(&regs->DIR, val));
}

inline static void lm4f_gpio_den(struct lm4f_gpio *gpio, uint8_t enable) {
    struct lm4f_gpio_regs *regs = gpio_get_regs(gpio->port);
    uint32_t val;

    do {
        val = load_link(&regs->DEN);

        val &= ~(1 << gpio->pin);
        val |= (enable << gpio->pin);
    } while (store_conditional(&regs->DEN, val));
}

enum lm4f_gpio_directions {
    LM4F_GPIO_DIR_IN = 0,
    LM4F_GPIO_DIR_OUT,
};

/* Enumerate all of the GPIOs */
enum lm4f_gpios {
    LM4F_GPIO_PA0 = 0,
    LM4F_GPIO_PA1,
    LM4F_GPIO_PA2,
    LM4F_GPIO_PA3,
    LM4F_GPIO_PA4,
    LM4F_GPIO_PA5,
    LM4F_GPIO_PA6,
    LM4F_GPIO_PA7,
    LM4F_GPIO_PB0,
    LM4F_GPIO_PB1,
    LM4F_GPIO_PB2,
    LM4F_GPIO_PB3,
    LM4F_GPIO_PB4,
    LM4F_GPIO_PB5,
    LM4F_GPIO_PB6,
    LM4F_GPIO_PB7,
    LM4F_GPIO_PC0,
    LM4F_GPIO_PC1,
    LM4F_GPIO_PC2,
    LM4F_GPIO_PC3,
    LM4F_GPIO_PC4,
    LM4F_GPIO_PC5,
    LM4F_GPIO_PC6,
    LM4F_GPIO_PC7,
    LM4F_GPIO_PD0,
    LM4F_GPIO_PD1,
    LM4F_GPIO_PD2,
    LM4F_GPIO_PD3,
    LM4F_GPIO_PD4,
    LM4F_GPIO_PD5,
    LM4F_GPIO_PD6,
    LM4F_GPIO_PD7,
    LM4F_GPIO_PE0,
    LM4F_GPIO_PE1,
    LM4F_GPIO_PE2,
    LM4F_GPIO_PE3,
    LM4F_GPIO_PE4,
    LM4F_GPIO_PE5,
    LM4F_GPIO_PF0,
    LM4F_GPIO_PF1,
    LM4F_GPIO_PF2,
    LM4F_GPIO_PF3,
    LM4F_GPIO_PF4,
    LM4F_NUM_GPIOS,
};

#endif
