#include <stdint.h>
#include <stddef.h>
#include <dev/registers.h>
#include <dev/resource.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <dev/hw/i2c.h>

#define I2C1_SDA    9
#define I2C1_SCL    8

uint8_t i2c1_write(uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));
uint8_t i2c1_read(uint8_t addr) __attribute__((section(".kernel")));
uint8_t i2cnowrite(uint8_t addr, uint8_t *data, uint32_t num) __attribute__((section(".kernel")));
uint8_t i2cnoread(uint8_t addr) __attribute__((section(".kernel")));
void i2c1_stop(void) __attribute__((section(".kernel")));

struct i2c_dev i2c1 = {
    .read = &i2cnoread,
    .write = &i2cnowrite
};

struct semaphore i2c1_semaphore = {
    .lock = 0,
    .held_by = NULL,
    .waiting = NULL
};

uint8_t i2cnowrite(uint8_t addr, uint8_t *data, uint32_t num) {
        panic_print("Attempted write on uninitialized i2c device.\r\n");
        /* Execution will never reach here */
        return -1;
}

uint8_t i2cnoread(uint8_t addr) {
        panic_print("Attempted read on uninitialized i2c device.\r\n");
        /* Execution will never reach here */
        return -1;
}

/* This has to be a function because GCC's optimizations suck 
 * GCC's optimizations break read and write when using this line
 * on their own or with an inline function, despite the fact that
 * GCC inlines this function anyway. */
void i2c1_stop(void) {
    *I2C1_CR1 |= I2C_CR1_STOP;
}

void init_i2c(void) {
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
    *I2C1_CR2 |= I2C_CR2_FREQ(42);

    /* Should set I2C to 100kHz */
    *I2C1_CCR |= I2C_CCR_CCR(210);
    *I2C1_TRISE = 42;

    /* Enable */
    *I2C1_CR1 |= I2C_CR1_PE;

    init_semaphore(&i2c1_semaphore);

    i2c1.read = &i2c1_read;
    i2c1.write = &i2c1_write;
}

uint8_t i2c1_write(uint8_t addr, uint8_t *data, uint32_t num) {
    *I2C1_CR1 |= I2C_CR1_START;

    int count = 10000;
    while (!(*I2C1_SR1 & I2C_SR1_SB)) {
        if (!count--) {
            return -1;
        }
    }

    *I2C1_DR = addr << 1;

    while (!(*I2C1_SR1 & I2C_SR1_ADDR)) {
        if (*I2C1_SR1 & I2C_SR1_AF) {
            return -1;
        }
    }

    while (!(*I2C1_SR2 & I2C_SR2_MSL)); 

    while (num--) {
        *I2C1_DR = *data++;

        while (!(*I2C1_SR1 & I2C_SR1_TXE));
    }

    i2c1_stop();

    return 0;
}

uint8_t i2c1_read(uint8_t addr) {
    uint8_t data;

    *I2C1_CR1 |= I2C_CR1_START;

    int count = 10000;
    while (!(*I2C1_SR1 & I2C_SR1_SB)) {
        if (!count--) {
            return 69;
        }
    }

    *I2C1_DR = (addr << 1) | 1;

    while (!(*I2C1_SR1 & I2C_SR1_ADDR)) {
        if (*I2C1_SR1 & I2C_SR1_AF) {
            return 69;
        }
    }

    while (!(*I2C1_SR2 & I2C_SR2_MSL)); 
    while (!(*I2C1_SR1 & I2C_SR1_RXNE));

    data = *I2C1_DR;

    return data;

    i2c1_stop();
}
