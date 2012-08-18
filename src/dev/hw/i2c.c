#include "dev_header.h"
#include "i2c.h"

//struct i2c_dev i2c1 = {
//    .read = &i2cnoread,
//    .write = *i2cnowrite
//};

void init_i2c1(void) {
    *RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;     /* Enable I2C1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;    /* Enable GPIOB Clock */

    /* Set PB8 and PB9 to alternative function I2C
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* Sets PB8 and PB9 to alternative function mode */
    *GPIOB_MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    *GPIOB_MODER |= (GPIO_MODER_ALT << (8 * 2)) | (GPIO_MODER_ALT << (9 * 2));

    /* Sets PB8 and PB9 to I2C mode */
    *GPIOB_AFRH  &= ~((0xF << ((8-8) * 4)) | (0xF << ((9-8) * 4)));
    *GPIOB_AFRH  |= (GPIO_AF_I2C << ((8-8) * 4)) | (GPIO_AF_I2C << ((9-8) * 4));

    /* Sets pin output to open drain */
    *GPIOB_OTYPER |= (1 << 8) | (1 << 9);

    /* No pull-up, no pull-down */
    *GPIOB_PUPDR  &= ~((3 << (8 * 2)) | (3 << (9 * 2)));

    /* Speed to 50Mhz */
    *GPIOB_OSPEEDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    *GPIOB_OSPEEDR |= (2 << (8 * 2)) | (2 << (9 * 2));

    /* Configure peripheral */
    *I2C1_CR2 |= I2C_CR2_FREQ(42);

    /* Should set I2C to 100kHz */
    *I2C1_CCR |= I2C_CCR_CCR(210);
    *I2C1_TRISE = 42;

    /* Enable */
    *I2C1_CR1 |= I2C_CR1_PE;

    //i2c1.read = &i2c1read;
    //i2c1.write = &i2c1write;
}

void i2c1_write(uint8_t addr, uint8_t data) {
    *I2C1_CR1 |= I2C_CR1_START;

    while (!(*I2C1_SR1 & I2C_SR1_SB));

    *I2C1_DR = addr;

    uint32_t count = 10000;
    while (!(*I2C1_SR1 & I2C_SR1_ADDR)) {
        if (!count) {
            printf("Couldn't write from address: 0x%x\r\n", addr);
            return;
        }
        else if (*I2C1_SR1 & I2C_SR1_AF) {
            printf("Acknowledge failure: 0x%x\r\n", addr);
            return;
        }
        count--;
    }

    printf("Acknowledge received: 0x%x\r\n", addr);

    while (!(*I2C1_SR2 & I2C_SR2_MSL)); 

    *I2C1_DR = data;

    while (!(*I2C1_SR1 & I2C_SR1_TXE));

    printf("Written to address: 0x%x\r\n", addr);
}

uint8_t i2c1_read(uint8_t addr) {
    uint8_t data;

    *I2C1_CR1 |= I2C_CR1_START;
    /* No ACK */
    //*I2C1_CR1 &= ~(I2C_CR1_ACK);

    while (!(*I2C1_SR1 & I2C_SR1_SB));

    *I2C1_DR = addr;

    printf("addr = 0x%x ", addr);

    uint32_t count = 10000;
    while (!(*I2C1_SR1 & I2C_SR1_ADDR)) {
        if (!count) {
            printf("Couldn't read from address: 0x%x\r\n", addr);
            return 0;
        }
        else if (*I2C1_SR1 & I2C_SR1_AF) {
            printf("Acknowledge failure: 0x%x\r\n", addr);
            return 0;
        }
        count--;
    }

    printf("Acknowledge received: 0x%x\r\n", addr);
    
    while (!(*I2C1_SR2 & I2C_SR2_MSL)); 
    while (!(*I2C1_SR1 & I2C_SR1_RXNE));

    data = *I2C1_DR;

    return data;
}
