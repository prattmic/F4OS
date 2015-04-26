/*
 * Copyright (C) 2015 F4OS Authors
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

#include <atomic.h>
#include <stdint.h>
#include <libfdt.h>
#include <arch/chip/gpio.h>
#include <dev/fdtparse.h>
#include <dev/hw/gpio.h>
#include <dev/raw_mem.h>
#include <mm/mm.h>

#define MSP432_GPIO_COMPAT  "ti,msp432p401-gpio"

/*
 * MSP432 GPIOs are generally internally referred to by lettered ports that are
 * a combination of numbered ports.  For example, Port A contains Port 1 and
 * Port 2.  Here, 'port' refers to the lettered port, where PA = 0, PB = 1,
 * etc.
 *
 * For example, P4.5 => PB12 => port = 1, pin = 12.
 */

struct msp432_gpio {
    uint8_t port;
    uint8_t pin;
    struct msp432_gpio_regs *regs;
};

/* Atomically set only certain bits in a register. */
static inline void atomic_set_reg(uint16_t *reg, uint16_t mask, uint16_t bits) {
    uint16_t tmp;

    do {
        tmp = load_link16(reg);
        tmp &= ~mask;
        tmp |= bits;
    } while (store_conditional16(reg, tmp));
}

/*
 * Set to standard reset values:
 * input, push-pull, low speed, no pull up/down
 */
static int msp432_gpio_reset(struct gpio *g) {
    struct msp432_gpio *gpio = g->priv;

    /* Interrupts disabled */
    atomic_set_reg(&gpio->regs->IE, 1 << gpio->pin, 0);

    /* Pin to input */
    atomic_set_reg(&gpio->regs->DIR, 1 << gpio->pin,
                   MSP432_GPIO_DIR_IN << gpio->pin);

    /* Pull-(up|down) disabled */
    atomic_set_reg(&gpio->regs->REN, 1 << gpio->pin, 0);

    /* Pull-down selected (no-op) */
    atomic_set_reg(&gpio->regs->OUT, 1 << gpio->pin, 0);

    /* Regular drive strength */
    atomic_set_reg(&gpio->regs->DS, 1 << gpio->pin, 0);

    /* No module function */
    atomic_set_reg(&gpio->regs->SEL0, 1 << gpio->pin, 0);
    atomic_set_reg(&gpio->regs->SEL1, 1 << gpio->pin, 0);

    g->active_low = 0;

    return 0;
}

static int msp432_gpio_active_low(struct gpio *g, int active_low) {
    /* Force active_low to a binary value */
    g->active_low = !!active_low;

    return 0;
}

static int msp432_gpio_direction(struct gpio *g, int flags) {
    struct msp432_gpio *gpio = g->priv;
    int mode;

    switch (flags & GPIO_DIRECTION_MASK) {
    case GPIO_INPUT:
        mode = MSP432_GPIO_DIR_IN;
        break;
    case GPIO_OUTPUT:
        mode = MSP432_GPIO_DIR_OUT;
        break;
    default:
        return -1;
    }

    atomic_set_reg(&gpio->regs->DIR, 1 << gpio->pin, mode << gpio->pin);

    return 0;
}

static unsigned int msp432_gpio_get_input_value(struct gpio *g) {
    struct msp432_gpio *gpio = g->priv;

    uint16_t reg = raw_mem_read(&gpio->regs->IN);

    /* Flip value if active low */
    return ((reg >> gpio->pin) & 1) ^ g->active_low;
}

static int msp432_gpio_set_output_value(struct gpio *g, unsigned int val) {
    struct msp432_gpio *gpio = g->priv;

    /* Force val to 0 or 1, flip if active low */
    val = !!val ^ g->active_low;

    atomic_set_reg(&gpio->regs->OUT, 1 << gpio->pin, val << gpio->pin);

    return 0;
}

static unsigned int msp432_gpio_get_output_value(struct gpio *g) {
    struct msp432_gpio *gpio = g->priv;

    uint16_t reg = raw_mem_read(&gpio->regs->OUT);

    /* Flip value if active low */
    return ((reg >> gpio->pin) & 1) ^ g->active_low;
}

static int msp432_gpio_set_function(struct gpio *g,
                                    enum msp432_gpio_mod_funcs func) {
    struct msp432_gpio *gpio = g->priv;
    uint16_t sel0, sel1;

    /*
     * The SEL1:SEL0 registers comprise a two bit function value, where GPIO is
     * 0, and primary, secondary, and tertiary are 1, 2, and 3, respectively.
     */

    switch (func) {
    case MSP432_GPIO_FUNCTION_GPIO:
        sel0 = 0;
        sel1 = 0;
        break;
    case MSP432_GPIO_FUNCTION_PRIMARY:
        sel0 = 1;
        sel1 = 0;
        break;
    case MSP432_GPIO_FUNCTION_SECONDARY:
        sel0 = 0;
        sel1 = 1;
        break;
    case MSP432_GPIO_FUNCTION_TERTIARY:
        sel0 = 0;
        sel1 = 1;
        break;
    default:
        return GPIO_ERR_FLG_INVAL;
    }

    atomic_set_reg(&gpio->regs->SEL0, 1 << gpio->pin, sel0 << gpio->pin);
    atomic_set_reg(&gpio->regs->SEL1, 1 << gpio->pin, sel1 << gpio->pin);

    return 0;
}

static int msp432_gpio_set_pull(struct gpio *g,
                                enum msp432_gpio_pull pull) {
    struct msp432_gpio *gpio = g->priv;
    uint16_t enable;
    uint16_t direction;

    switch (pull) {
    case MSP432_GPIO_PULL_DISABLE:
        enable = MSP432_GPIO_REN_DISABLE;
        direction = MSP432_GPIO_OUT_PULLDOWN;   /* Pull-down default */
        break;
    case MSP432_GPIO_PULL_UP:
        enable = MSP432_GPIO_REN_ENABLE;
        direction = MSP432_GPIO_OUT_PULLUP;
        break;
    case MSP432_GPIO_PULL_DOWN:
        enable = MSP432_GPIO_REN_ENABLE;
        direction = MSP432_GPIO_OUT_PULLDOWN;
        break;
    default:
        return GPIO_ERR_FLG_INVAL;
    }

    atomic_set_reg(&gpio->regs->OUT, 1 << gpio->pin, direction << gpio->pin);
    atomic_set_reg(&gpio->regs->REN, 1 << gpio->pin, enable << gpio->pin);

    return 0;
}

/*
 * Valid flags:
 * MSP432_GPIO_MOD_FUNC:
 *  set GPIO to module function val
 *  @param val  Module function from enum msp432_gpio_mod_funcs
 *  @returns zero on success, negative on error
 * MSP432_GPIO_PULL:
 *  set GPIO pull up/down, or disable pull up/down.  Only defined when
 *  configured as input.
 *  @param val  Pull up/down setting from enum msp432_gpio_pull
 *  @returns zero on success, negative on error
 */
static int msp432_gpio_set_flags(struct gpio *g, unsigned int flag, int val) {
    switch (flag) {
    case MSP432_GPIO_MOD_FUNC:
        return msp432_gpio_set_function(g, val);
    case MSP432_GPIO_PULL:
        return msp432_gpio_set_pull(g, val);
    default:
        return GPIO_ERR_FLG_UNSUPPT;
    }
}

static int msp432_gpio_dtor(struct gpio *gpio) {
    msp432_gpio_reset(gpio);

    kfree(gpio->priv);

    return 0;
}

static struct gpio_ops msp432_gpio_ops = {
    .reset = msp432_gpio_reset,
    .active_low = msp432_gpio_active_low,
    .direction = msp432_gpio_direction,
    .get_input_value = msp432_gpio_get_input_value,
    .set_output_value = msp432_gpio_set_output_value,
    .get_output_value = msp432_gpio_get_output_value,
    .set_flags = msp432_gpio_set_flags,
    .dtor = msp432_gpio_dtor,
};

/*
 * Build canonical, external GPIO name, in the form P1.7.
 * Buffer must be at least 5 bytes long.
 */
static void msp432_gpio_build_name(enum msp432_gpios num, char *buf) {
    uint8_t port = num / 8;
    uint8_t pin = num % 8;

    buf[0] = 'P';
    buf[1] = '0' + port;
    buf[2] = '.';
    buf[3] = '0' + pin;
    buf[4] = '\0';
}

/* Implement exported interface defined in dev/hw/gpio.h */
int gpio_valid(uint32_t gpio) {
    if (gpio < MSP432_NUM_GPIOS) {
        return 0;
    }

    return GPIO_ERR_INVAL;
}

struct obj *_gpio_instantiate(uint32_t num) {
    const void *blob = fdtparse_get_blob();
    int offset;
    struct msp432_gpio_regs *regs;
    struct obj *obj;
    struct gpio *gpio;
    struct msp432_gpio *msp432_gpio;
    char name[5] = {'\0'};

    /* TODO: refactor GPIO interface to allow lookup by GPIO node phandle */
    offset = fdt_node_offset_by_compatible(blob, 0, MSP432_GPIO_COMPAT);
    if (offset < 0) {
        return NULL;
    }

    /*
     * Points to beginning of Port A registers.  Later ports follow immediately
     * afterwards.
     */
    regs = fdtparse_get_addr32(blob, offset, "regs");
    if (!regs) {
        return NULL;
    }

    msp432_gpio_build_name(num, name);

    obj = instantiate(name, &gpio_class, &msp432_gpio_ops, struct gpio);
    if (!obj) {
        return NULL;
    }

    gpio = to_gpio(obj);

    gpio->priv = kmalloc(sizeof(struct msp432_gpio));
    if (!gpio->priv) {
        goto err_free_obj;
    }

    msp432_gpio = gpio->priv;
    /* Here we refer to 16-pin lettered ports. */
    msp432_gpio->port = num / 16;
    msp432_gpio->pin = num % 16;
    /* Each port has a register bank, point directly to this port's bank */
    msp432_gpio->regs = &regs[msp432_gpio->port];

    /* Export to the OS */
    class_export_member(obj);

    return obj;

err_free_obj:
    class_deinstantiate(obj);

    return NULL;
}
