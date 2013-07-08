#include <stddef.h>
#include <stdint.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <dev/resource.h>
#include <kernel/fault.h>
#include <kernel/semaphore.h>

#include <dev/hw/i2c.h>

static void init_i2c1(void) __attribute__((section(".kernel")));
static void init_i2c2(void) __attribute__((section(".kernel")));
static int i2c_reset(struct i2c_dev *i2c);
static int i2c_force_clear_busy(struct i2c_dev *i2c);

void i2c_stop(uint8_t port) __attribute__((section(".kernel")));

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

/* This has to be a function because GCC's optimizations suck
 * GCC's optimizations break read and write when using this line
 * on their own or with an inline function, despite the fact that
 * GCC inlines this function anyway. */
void i2c_stop(uint8_t port) {
    *I2C_CR1(port) |= I2C_CR1_STOP;
}

#define I2C1_SCL    8
#define I2C1_SDA    9

#define I2C2_SCL    10
#define I2C2_SDA    11

static void init_i2c1(void) {
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
    *I2C_CR2(1) |= I2C_CR2_FREQ(42);

    /* Set I2C to 300kHz */
    *I2C_CCR(1) |= I2C_CCR_CCR(140);
    *I2C_TRISE(1) = 43;

    /* Enable */
    *I2C_CR1(1) |= I2C_CR1_PE;

    /* Pre-initialized */
    //init_semaphore(&i2c1_semaphore);

    i2c1.ready = 1;
}

static void init_i2c2(void) {
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
    *I2C_CR2(2) |= I2C_CR2_FREQ(42);

    /* Set I2C to 300kHz */
    *I2C_CCR(2) |= I2C_CCR_CCR(140);
    *I2C_TRISE(2) = 43;

    /* Enable */
    *I2C_CR1(2) |= I2C_CR1_PE;

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

    i2c->ready = 0;

    /* Software reset */
    *I2C_CR1(i2c->port) |= I2C_CR1_SWRST;

    /* Wait until peripheral is disabled */
    while (*I2C_CR1(i2c->port) & I2C_CR1_PE);

    /* Re-enable */
    *I2C_CR1(i2c->port) &= ~(I2C_CR1_SWRST);

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
    if (*I2C_SR2(i2c->port) & I2C_SR2_BUSY) {
        /* Last ditch effort */
        if (i2c_reset(i2c) || (*I2C_SR2(i2c->port) & I2C_SR2_BUSY)) {
            /* Failed to reset */
            printk("I2C: BUSY flag failed to clear.\r\nI2C: I have tried everything I know :(. At this point, reset is your best option.\r\n");
            return -1;
        }
    }

    return 0;
}

int8_t i2c_write(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num) {
    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3 || !data || !num) {
        return -1;
    }

    /* Check for bus error */
    if (*I2C_SR1(i2c->port) & I2C_SR1_BERR) {
        printk("I2C: Bus error, reseting.\r\n");
        /* Clear the error and reset I2C */
        *I2C_SR1(i2c->port) &= ~(I2C_SR1_BERR);

        if (i2c_reset(i2c)) {
            /* Failed to reset */
            return -1;
        }
    }

    /* Wait until BUSY is reset and previous transaction STOP is complete */
    int count = 10000;
    while ((*I2C_SR2(i2c->port) & I2C_SR2_BUSY) || (*I2C_CR1(i2c->port) & I2C_CR1_STOP)) {
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

    *I2C_CR1(i2c->port) |= I2C_CR1_START;

    count = 10000;
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_SB)) {
        if (!count--) {
            i2c_stop(i2c->port);
            return -1;
        }
    }

    *I2C_DR(i2c->port) = addr << 1;

    count = 10000;
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_ADDR)) {
        if ((*I2C_SR1(i2c->port) & I2C_SR1_AF) || !count--) {
            i2c_stop(i2c->port);
            return -1;
        }
    }

    count = 10000;
    while (!(*I2C_SR2(i2c->port) & I2C_SR2_MSL)) {
        if (!count--) {
            i2c_stop(i2c->port);
            return -1;
        }
    }

    int total = 0;

    while (num--) {
        /* Make sure shift register is empty */
        count = 10000;
        while (!(*I2C_SR1(i2c->port) & I2C_SR1_TXE)) {
            if (!count--) {
                i2c_stop(i2c->port);
                return -1;
            }
        }

        *I2C_DR(i2c->port) = *data++;

        count = 10000;
        while (!(*I2C_SR1(i2c->port) & I2C_SR1_TXE)) {
            if (!count--) {
                i2c_stop(i2c->port);
                return -1;
            }
        }

        total += 1;
    }

    i2c_stop(i2c->port);

    return total;
}

int i2c_read(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num) {
    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3 || !data || !num) {
        return -1;
    }

    /* Check for bus error */
    if (*I2C_SR1(i2c->port) & I2C_SR1_BERR) {
        printk("I2C: Bus error, reseting.\r\n");
        /* Clear the error and reset I2C */
        *I2C_SR1(i2c->port) &= ~(I2C_SR1_BERR);

        if (i2c_reset(i2c)) {
            /* Failed to reset */
            return -1;
        }
    }

    /* Wait until BUSY is reset and previous transaction STOP is complete */
    int count = 10000;
    while ((*I2C_SR2(i2c->port) & I2C_SR2_BUSY) || (*I2C_CR1(i2c->port) & I2C_CR1_STOP)) {
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

    *I2C_CR1(i2c->port) |= I2C_CR1_START;

    count = 10000;
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_SB)) {
        if (!count--) {
            i2c_stop(i2c->port);
            return -1;
        }
    }

    *I2C_DR(i2c->port) = (addr << 1) | 1;

    count = 10000;
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_ADDR)) {
        if (*I2C_SR1(i2c->port) & I2C_SR1_AF || !count--) {
            i2c_stop(i2c->port);
            return -1;
        }
    }

    uint8_t single_byte = num == 1;

    if (!single_byte) {
        *I2C_CR1(i2c->port) |= I2C_CR1_ACK;
    }
    else {  /* In single byte receive, never ACK */
        *I2C_CR1(i2c->port) &= ~(I2C_CR1_ACK);
    }

    while (num--) {
        count = 10000;
        while (!(*I2C_SR2(i2c->port) & I2C_SR2_MSL)) {
            if (!count--) {
                i2c_stop(i2c->port);
                return -1;
            }
        }

        /* In single byte receive, stop after ADDR clear (SR1 and SR2 read) */
        if (single_byte) {
            i2c_stop(i2c->port);
        }

        count = 10000;
        while (!(*I2C_SR1(i2c->port) & I2C_SR1_RXNE)) {
            if (!count--) {
                i2c_stop(i2c->port);
                return -1;
            }
        }

        *data++ = *I2C_DR(i2c->port);
        total++;

        /* NACK and STOP after second last receive */
        if (num == 1) {
            *I2C_CR1(i2c->port) &= ~(I2C_CR1_ACK);
            i2c_stop(i2c->port);
        }
    }

    return total;
}
