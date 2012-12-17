#include <stdint.h>
#include <stddef.h>
#include <dev/registers.h>
#include <dev/resource.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <dev/hw/i2c.h>

static void init_i2c1(void) __attribute__((section(".kernel")));
static void init_i2c2(void) __attribute__((section(".kernel")));

void i2c_stop(uint8_t port) __attribute__((section(".kernel")));

struct i2c_dev i2c1 = {
    .ready = 0,
    .port = 1,
    .init = &init_i2c1
};

struct semaphore i2c1_semaphore = {
    .lock = 0,
    .held_by = NULL,
    .waiting = NULL
};

struct i2c_dev i2c2 = {
    .ready = 0,
    .port = 2,
    .init = &init_i2c2
};

struct semaphore i2c2_semaphore = {
    .lock = 0,
    .held_by = NULL,
    .waiting = NULL
};

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

    /* Should set I2C to 100kHz */
    *I2C_CCR(1) |= I2C_CCR_CCR(210);
    *I2C_TRISE(1) = 42;

    /* Enable */
    *I2C_CR1(1) |= I2C_CR1_PE;

    init_semaphore(&i2c1_semaphore);

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

    /* Should set I2C to 100kHz */
    *I2C_CCR(2) |= I2C_CCR_CCR(210);
    *I2C_TRISE(2) = 42;

    /* Enable */
    *I2C_CR1(2) |= I2C_CR1_PE;

    init_semaphore(&i2c2_semaphore);

    i2c2.ready = 1;
}

int8_t i2c_write(struct i2c_dev *i2c, uint8_t addr, uint8_t *data, uint32_t num) {
    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3) {
        return -1;
    }

    *I2C_CR1(i2c->port) |= I2C_CR1_START;

    int count = 10000;
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

    while (!(*I2C_SR2(i2c->port) & I2C_SR2_MSL)); 

    while (num--) {
        *I2C_DR(i2c->port) = *data++;

        while (!(*I2C_SR1(i2c->port) & I2C_SR1_TXE));
    }

    i2c_stop(i2c->port);

    return 0;
}

uint8_t i2c_read(struct i2c_dev *i2c, uint8_t addr, int *error) {
    if (!i2c || !i2c->ready || i2c->port < 1 || i2c->port > 3) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    if (error != NULL) {
        *error = 0;
    }

    uint8_t data;

    *I2C_CR1(i2c->port) |= I2C_CR1_START;

    int count = 10000;
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_SB)) {
        if (!count--) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }
    }

    *I2C_DR(i2c->port) = (addr << 1) | 1;

    while (!(*I2C_SR1(i2c->port) & I2C_SR1_ADDR)) {
        if (*I2C_SR1(i2c->port) & I2C_SR1_AF) {
            if (error != NULL) {
                *error = -1;
            }
            return 0;
        }
    }

    while (!(*I2C_SR2(i2c->port) & I2C_SR2_MSL)); 
    while (!(*I2C_SR1(i2c->port) & I2C_SR1_RXNE));

    data = *I2C_DR(i2c->port);

    i2c_stop(i2c->port);

    return data;
}
