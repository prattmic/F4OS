#include <stddef.h>
#include <stdint.h>
#include <arch/chip/gpio.h>
#include <arch/chip/i2c.h>
#include <arch/chip/registers.h>
#include <dev/raw_mem.h>
#include <dev/resource.h>
#include <kernel/fault.h>
#include <kernel/semaphore.h>

#include <dev/hw/i2c.h>

static void init_i2c1(void) __attribute__((section(".kernel")));
static void init_i2c2(void) __attribute__((section(".kernel")));
static int i2c_reset(struct i2c_dev *i2c);
static int i2c_force_clear_busy(struct i2c_dev *i2c);

struct i2c_dev i2c1 = {
    .ready = 0,
    .port = 1,
    .init = &init_i2c1
};

struct semaphore i2c1_semaphore = INIT_SEMAPHORE;

struct i2c_dev i2c2 = {
    .ready = 0,
    .port = 2,
    .init = &init_i2c2
};

struct semaphore i2c2_semaphore = INIT_SEMAPHORE;

#define I2C1_SCL    8
#define I2C1_SDA    9

#define I2C2_SCL    10
#define I2C2_SDA    11

static void init_i2c1(void) {
    struct stm32f4_i2c_regs *regs = i2c_get_regs(1);

    *RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;     /* Enable I2C1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;    /* Enable GPIOB Clock */

    /* Set PB8 and PB9 to alternative function I2C
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* I2C1_SCL */
    gpio_moder(GPIOB, I2C1_SCL, GPIO_MODER_ALT);
    gpio_afr(GPIOB, I2C1_SCL, GPIO_AF_I2C);
    gpio_otyper(GPIOB, I2C1_SCL, GPIO_OTYPER_OD);
    gpio_pupdr(GPIOB, I2C1_SCL, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, I2C1_SCL, GPIO_OSPEEDR_50M);

    /* I2C1_SDA */
    gpio_moder(GPIOB, I2C1_SDA, GPIO_MODER_ALT);
    gpio_afr(GPIOB, I2C1_SDA, GPIO_AF_I2C);
    gpio_otyper(GPIOB, I2C1_SDA, GPIO_OTYPER_OD);
    gpio_pupdr(GPIOB, I2C1_SDA, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, I2C1_SDA, GPIO_OSPEEDR_50M);

    /* Configure peripheral */
    raw_mem_set_mask(&regs->CR2, I2C_CR2_FREQ_MASK, I2C_CR2_FREQ(42));

    /* Set I2C to 300kHz */
    raw_mem_set_mask(&regs->CCR, I2C_CCR_CCR_MASK, I2C_CCR_CCR(140));
    raw_mem_write(&regs->TRISE, 43);

    /* Enable */
    raw_mem_set_bits(&regs->CR1, I2C_CR1_PE);

    /* Pre-initialized */
    //init_semaphore(&i2c1_semaphore);

    i2c1.ready = 1;
}

static void init_i2c2(void) {
    struct stm32f4_i2c_regs *regs = i2c_get_regs(2);

    *RCC_APB1ENR |= RCC_APB1ENR_I2C2EN;     /* Enable I2C2 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;    /* Enable GPIOB Clock */

    /* Set PB8 and PB9 to alternative function I2C
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* I2C2_SCL */
    gpio_moder(GPIOB, I2C2_SCL, GPIO_MODER_ALT);
    gpio_afr(GPIOB, I2C2_SCL, GPIO_AF_I2C);
    gpio_otyper(GPIOB, I2C2_SCL, GPIO_OTYPER_OD);
    gpio_pupdr(GPIOB, I2C2_SCL, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, I2C2_SCL, GPIO_OSPEEDR_50M);

    /* I2C2_SDA */
    gpio_moder(GPIOB, I2C2_SDA, GPIO_MODER_ALT);
    gpio_afr(GPIOB, I2C2_SDA, GPIO_AF_I2C);
    gpio_otyper(GPIOB, I2C2_SDA, GPIO_OTYPER_OD);
    gpio_pupdr(GPIOB, I2C2_SDA, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, I2C2_SDA, GPIO_OSPEEDR_50M);

    /* Configure peripheral */
    raw_mem_set_mask(&regs->CR2, I2C_CR2_FREQ_MASK, I2C_CR2_FREQ(42));

    /* Set I2C to 300kHz */
    raw_mem_set_mask(&regs->CCR, I2C_CCR_CCR_MASK, I2C_CCR_CCR(140));
    raw_mem_write(&regs->TRISE, 43);

    /* Enable */
    raw_mem_set_bits(&regs->CR1, I2C_CR1_PE);

    /* Pre-initialized */
    //init_semaphore(&i2c2_semaphore);

    i2c2.ready = 1;
}

/* In the event of a bus error (start or stop detected during data transfer),
 * the peripheral continues as normal, but the slave has likely cancelled
 * communication, leaving the peripheral expecting communication that won't
 * occur.  The only solution is to reset the peripheral. */
static int i2c_reset(struct i2c_dev *i2c) {
    if (!i2c) {
        return -1;
    }

    struct stm32f4_i2c_regs *regs = i2c_get_regs(i2c->port);

    i2c->ready = 0;

    /* Software reset */
    raw_mem_set_bits(&regs->CR1, I2C_CR1_SWRST);

    /* Wait until peripheral is disabled */
    while (raw_mem_read(&regs->CR1) & I2C_CR1_PE);

    /* Re-enable */
    raw_mem_clear_bits(&regs->CR1, I2C_CR1_SWRST);

    /* Re-initialize */
    i2c->init();

    return 0;
}

/* On rare occassions, the I2C device will get confused, either because we missed
 * a timing requirement, or it is just stupid.  Regardless, it holds SDA low waiting
 * for some unknown action from the master.  This keeps the bus BUSY and prevents
 * any further communication.  This condition is fixed by manually clocking SCL
 * until SDA is released by the slave.  As far as it is concerned, we just completed
 * a normal transaction. */
static int i2c_force_clear_busy(struct i2c_dev *i2c) {
    if (!i2c) {
        return -1;
    }

    struct stm32f4_i2c_regs *regs = i2c_get_regs(i2c->port);

    int count = 10000;

    switch (i2c->port) {
    case 1:
        /* Set pins to output/input */
        gpio_moder(GPIOB, I2C1_SCL, GPIO_MODER_OUT);
        gpio_moder(GPIOB, I2C1_SDA, GPIO_MODER_IN);

        /* Toggle clock until bus no longer busy */
        while (!(*GPIO_IDR(GPIOB) & GPIO_IDR_PIN(I2C1_SDA))) {
            if (!count--) {
                /* Out of time, perhaps the last ditch effort will save us. */
                break;
            }

            /* Toggle clock */
            *GPIO_ODR(GPIOB) ^= GPIO_ODR_PIN(I2C1_SCL);
            for (volatile int delay = 100; delay > 0; delay--);
        }

        gpio_moder(GPIOB, I2C1_SCL, GPIO_MODER_ALT);
        gpio_moder(GPIOB, I2C1_SDA, GPIO_MODER_ALT);
        for (volatile int delay = 100; delay > 0; delay--);

        break;
    case 2:
        /* Set pins to output/input */
        gpio_moder(GPIOB, I2C2_SCL, GPIO_MODER_OUT);
        gpio_moder(GPIOB, I2C2_SDA, GPIO_MODER_IN);

        /* Toggle clock until SDA raised */
        while (!(*GPIO_IDR(GPIOB) & GPIO_IDR_PIN(I2C2_SDA))) {
            if (!count--) {
                /* Out of time, perhaps the last ditch effort will save us. */
                break;
            }

            /* Toggle clock */
            *GPIO_ODR(GPIOB) ^= GPIO_ODR_PIN(I2C2_SCL);
            for (volatile int delay = 100; delay > 0; delay--);
        }

        gpio_moder(GPIOB, I2C2_SCL, GPIO_MODER_ALT);
        gpio_moder(GPIOB, I2C2_SDA, GPIO_MODER_ALT);
        for (volatile int delay = 100; delay > 0; delay--);

        break;
    default:
        return -1;
    }

    /* Make sure the peripheral recognizes that the bus is now free */
    if (raw_mem_read(&regs->SR2) & I2C_SR2_BUSY) {
        /* Last ditch effort */
        if (i2c_reset(i2c) || (raw_mem_read(&regs->SR2) & I2C_SR2_BUSY)) {
            /* Failed to reset */
            printk("I2C: BUSY flag failed to clear.\r\nI2C: I have tried everything I know :(. At this point, reset is your best option.\r\n");
            return -1;
        }
    }

    return 0;
}

int8_t i2c_write(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num) {
    int ret;

    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3 || !data || !num) {
        return -1;
    }

    struct stm32f4_i2c_regs *regs = i2c_get_regs(i2c->port);

    /* Check for bus error */
    if (raw_mem_read(&regs->SR1) & I2C_SR1_BERR) {
        printk("I2C: Bus error, reseting.\r\n");
        /* Clear the error and reset I2C */
        raw_mem_clear_bits(&regs->SR1, I2C_SR1_BERR);

        if (i2c_reset(i2c)) {
            /* Failed to reset */
            return -1;
        }
    }

    /* Wait until BUSY is reset and previous transaction STOP is complete */
    int count = 10000;
    while ((raw_mem_read(&regs->SR2) & I2C_SR2_BUSY) ||
            (raw_mem_read(&regs->CR1) & I2C_CR1_STOP)) {
        if (--count == 0) {
            printk("I2C: Stalled, reseting.\r\n");

            if (i2c_reset(i2c)) {
                /* Failed to reset */
                return -1;
            }
        }
        else if (count < 0) {
            printk("I2C: Stalled, reset failed, force clearing busy.\r\n");

            if (i2c_force_clear_busy(i2c)) {
                /* Failed to clear */
                return -1;
            }
        }
    }

    raw_mem_set_bits(&regs->CR1, I2C_CR1_START);

    count = 10000;
    while (!(raw_mem_read(&regs->SR1) & I2C_SR1_SB)) {
        if (!count--) {
            ret = -1;
            goto out;
        }
    }

    raw_mem_write(&regs->DR, addr << 1);

    count = 10000;
    while (!(raw_mem_read(&regs->SR1) & I2C_SR1_ADDR)) {
        if ((raw_mem_read(&regs->SR1) & I2C_SR1_AF) || !count--) {
            ret = -1;
            goto out;
        }
    }

    count = 10000;
    while (!(raw_mem_read(&regs->SR2) & I2C_SR2_MSL)) {
        if (!count--) {
            ret = -1;
            goto out;
        }
    }

    int total = 0;

    while (num--) {
        /* Make sure shift register is empty */
        count = 10000;
        while (!(raw_mem_read(&regs->SR1) & I2C_SR1_TXE)) {
            if (!count--) {
                ret = -1;
                goto out;
            }
        }

        raw_mem_write(&regs->DR, *data++);

        count = 10000;
        while (!(raw_mem_read(&regs->SR1) & I2C_SR1_TXE)) {
            if (!count--) {
                ret = -1;
                goto out;
            }
        }

        total += 1;
    }

    ret = total;


out:
    raw_mem_set_bits(&regs->CR1, I2C_CR1_STOP);

    return ret;
}

int i2c_read(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num) {
    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3 || !data || !num) {
        return -1;
    }

    struct stm32f4_i2c_regs *regs = i2c_get_regs(i2c->port);

    /* Check for bus error */
    if (raw_mem_read(&regs->SR1) & I2C_SR1_BERR) {
        printk("I2C: Bus error, reseting.\r\n");
        /* Clear the error and reset I2C */
        raw_mem_clear_bits(&regs->SR1, I2C_SR1_BERR);

        if (i2c_reset(i2c)) {
            /* Failed to reset */
            return -1;
        }
    }

    /* Wait until BUSY is reset and previous transaction STOP is complete */
    int count = 10000;
    while ((raw_mem_read(&regs->SR2) & I2C_SR2_BUSY) ||
            (raw_mem_read(&regs->CR1) & I2C_CR1_STOP)) {
        if (--count == 0) {
            printk("I2C: Stalled, reseting.\r\n");

            if (i2c_reset(i2c)) {
                /* Failed to reset */
                return -1;
            }
        }
        else if (count < 0) {
            printk("I2C: Stalled, reset failed, force clearing busy.\r\n");

            if (i2c_force_clear_busy(i2c)) {
                /* Failed to clear */
                return -1;
            }
        }
    }

    int total = 0;

    raw_mem_set_bits(&regs->CR1, I2C_CR1_START);

    count = 10000;
    while (!(raw_mem_read(&regs->SR1) & I2C_SR1_SB)) {
        if (!count--) {
            goto out_err;
        }
    }

    raw_mem_write(&regs->DR, (addr << 1) | 1);

    count = 10000;
    while (!(raw_mem_read(&regs->SR1) & I2C_SR1_ADDR)) {
        if ((raw_mem_read(&regs->SR1) & I2C_SR1_AF) || !count--) {
            goto out_err;
        }
    }

    uint8_t single_byte = num == 1;

    if (!single_byte) {
        raw_mem_set_bits(&regs->CR1, I2C_CR1_ACK);
    }
    else {  /* In single byte receive, never ACK */
        raw_mem_clear_bits(&regs->CR1, I2C_CR1_ACK);
    }

    while (num--) {
        count = 10000;
        while (!(raw_mem_read(&regs->SR2) & I2C_SR2_MSL)) {
            if (!count--) {
                goto out_err;
            }
        }

        /* In single byte receive, stop after ADDR clear (SR1 and SR2 read) */
        if (single_byte) {
            raw_mem_set_bits(&regs->CR1, I2C_CR1_STOP);
        }

        count = 10000;
        while (!(raw_mem_read(&regs->SR1) & I2C_SR1_RXNE)) {
            if (!count--) {
                goto out_err;
            }
        }

        *data++ = raw_mem_read(&regs->DR);
        total++;

        /* NACK and STOP after second last receive */
        if (num == 1) {
            raw_mem_clear_bits(&regs->CR1, I2C_CR1_ACK);
            raw_mem_set_bits(&regs->CR1, I2C_CR1_STOP);
        }
    }

    return total;

out_err:
    raw_mem_set_bits(&regs->CR1, I2C_CR1_STOP);
    return -1;
}
