#ifndef ARCH_CHIP_INCLUDE_I2C_H
#define ARCH_CHIP_INCLUDE_I2C_H

#include <stdint.h>
#include <arch/chip/registers.h>

struct stm32f4_i2c_regs {
    uint32_t CR1;       /* I2Cx Control Register 1 */
    uint32_t CR2;       /* I2Cx Control Register 2 */
    uint32_t OAR1;      /* I2Cx Own Address Register 1 */
    uint32_t OAR2;      /* I2Cx Own Address Register 2 */
    uint32_t DR;        /* I2Cx Data Register */
    uint32_t SR1;       /* I2Cx Status Register 1 */
    uint32_t SR2;       /* I2Cx Status Register 2 */
    uint32_t CCR;       /* I2Cx Clock Control Register */
    uint32_t TRISE;     /* I2Cx TRISE Register */
    uint32_t FLTR;      /* I2Cx FLTR Register */
};

static inline struct stm32f4_i2c_regs *i2c_get_regs(int num) {
    switch (num) {
        case 1: return (struct stm32f4_i2c_regs *) I2C1_BASE;
        case 2: return (struct stm32f4_i2c_regs *) I2C2_BASE;
        case 3: return (struct stm32f4_i2c_regs *) I2C3_BASE;
    }
    return (struct stm32f4_i2c_regs *) INVALID_PERIPH_BASE;
}

#define I2C_CR1_PE          ((uint32_t) (1 << 0))       /* I2C peripheral enable */
#define I2C_CR1_SMBUS       ((uint32_t) (1 << 1))       /* I2C SMBus mode */
#define I2C_CR1_SMBTYPE     ((uint32_t) (1 << 3))       /* I2C SMBus type (0=Device, 1=Host) */
#define I2C_CR1_ENARP       ((uint32_t) (1 << 4))       /* I2C enable ARP */
#define I2C_CR1_ENPEC       ((uint32_t) (1 << 5))       /* I2C enable PEC */
#define I2C_CR1_ENGC        ((uint32_t) (1 << 6))       /* I2C enable general call */
#define I2C_CR1_NOSTRETCH   ((uint32_t) (1 << 7))       /* I2C clock stretching disable */
#define I2C_CR1_START       ((uint32_t) (1 << 8))       /* I2C START generation */
#define I2C_CR1_STOP        ((uint32_t) (1 << 9))       /* I2C STOP generation */
#define I2C_CR1_ACK         ((uint32_t) (1 << 10))      /* I2C ACK enable */
#define I2C_CR1_POS         ((uint32_t) (1 << 11))      /* I2C ACK/PEC position */
#define I2C_CR1_PEC         ((uint32_t) (1 << 12))      /* I2C packet error checking */
#define I2C_CR1_ALERT       ((uint32_t) (1 << 13))      /* I2C SMBus alert */
#define I2C_CR1_SWRST       ((uint32_t) (1 << 15))      /* I2C software reset */

#define I2C_CR2_FREQ(n)     ((uint32_t) (n << 0))       /* I2C clock frequency */
#define I2C_CR2_FREQ_MASK   ((uint32_t) (0x1F))         /* I2C clock frequency mask */
#define I2C_CR2_ITERREN     ((uint32_t) (1 << 8))       /* I2C error interrupt enable */
#define I2C_CR2_ITEVTEN     ((uint32_t) (1 << 9))       /* I2C event interrupt enable */
#define I2C_CR2_ITBUFEN     ((uint32_t) (1 << 10))      /* I2C buffer interrupt enable */
#define I2C_CR2_DMAEN       ((uint32_t) (1 << 11))      /* I2C DMA requests enable */
#define I2C_CR2_LAST        ((uint32_t) (1 << 12))      /* I2C DMA last transfer */

#define I2C_OAR1_ADD10(n)   ((uint32_t) (n << 0))       /* I2C interface address (10-bit) */
#define I2C_OAR1_ADD7(n)    ((uint32_t) (n << 1))       /* I2C interface address (7-bit) */
#define I2C_OAR1_ADDMODE    ((uint32_t) (1 << 15))      /* I2C interface address mode (1=10-bit) */

#define I2C_OAR2_ENDUAL     ((uint32_t) (1 << 0))       /* I2C dual address mode enable */
#define I2C_OAR2_ADD2(n)    ((uint32_t) (n << 1))       /* I2C interface address 2 (7-bit) */

#define I2C_SR1_SB          ((uint32_t) (1 << 0))       /* I2C start bit generated */
#define I2C_SR1_ADDR        ((uint32_t) (1 << 1))       /* I2C address sent/matched */
#define I2C_SR1_BTF         ((uint32_t) (1 << 2))       /* I2C byte transfer finished */
#define I2C_SR1_ADD10       ((uint32_t) (1 << 3))       /* I2C 10-bit header sent */
#define I2C_SR1_STOPF       ((uint32_t) (1 << 4))       /* I2C stop detection */
#define I2C_SR1_RXNE        ((uint32_t) (1 << 6))       /* I2C DR not empty */
#define I2C_SR1_TXE         ((uint32_t) (1 << 7))       /* I2C DR empty */
#define I2C_SR1_BERR        ((uint32_t) (1 << 8))       /* I2C bus error */
#define I2C_SR1_ARLO        ((uint32_t) (1 << 9))       /* I2C attribution lost */
#define I2C_SR1_AF          ((uint32_t) (1 << 10))      /* I2C acknowledge failure */
#define I2C_SR1_OVR         ((uint32_t) (1 << 11))      /* I2C overrun/underrun */
#define I2C_SR1_PECERR      ((uint32_t) (1 << 12))      /* I2C PEC error in reception */
#define I2C_SR1_TIMEOUT     ((uint32_t) (1 << 14))      /* I2C timeout or tlow error */
#define I2C_SR1_SMBALERT    ((uint32_t) (1 << 15))      /* I2C SMBus alert */

#define I2C_SR2_MSL         ((uint32_t) (1 << 0))       /* I2C master/slave */
#define I2C_SR2_BUSY        ((uint32_t) (1 << 1))       /* I2C bus busy */
#define I2C_SR2_TRA         ((uint32_t) (1 << 2))       /* I2C transmitter/receiver */
#define I2C_SR2_GENCALL     ((uint32_t) (1 << 4))       /* I2C general call address */
#define I2C_SR2_SMBDEFAUL   ((uint32_t) (1 << 5))       /* I2C SMBus device default address */
#define I2C_SR2_SMBHOST     ((uint32_t) (1 << 6))       /* I2C SMBus host header */
#define I2C_SR2_DUALF       ((uint32_t) (1 << 7))       /* I2C dual flag */
#define I2C_SR2_PEC(r)      ((uint32_t) (r >> 8))       /* I2C packet error checking register */

#define I2C_CCR_CCR(n)      ((uint32_t) (n & 0x0FFF))   /* I2C clock control register */
#define I2C_CCR_CCR_MASK    ((uint32_t) (0x0FFF))       /* I2C clock control register mask */
#define I2C_CCR_DUTY        ((uint32_t) (1 << 14))      /* I2C fast mode duty cycle */
#define I2C_CCR_FS          ((uint32_t) (1 << 15))      /* I2C master mode selection */

#endif
