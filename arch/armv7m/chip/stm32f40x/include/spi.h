#ifndef ARCH_CHIP_INCLUDE_SPI_H
#define ARCH_CHIP_INCLUDE_SPI_H

#include <arch/chip/registers.h>

typedef struct spi {
    volatile uint32_t CR1;      /* SPIx Control Register 1 */
    volatile uint32_t CR2;      /* SPIx Control Register 2 */
    volatile uint32_t SR;       /* SPIx Status Register */
    volatile uint32_t DR;       /* SPIx Data Register */
    volatile uint32_t CRCPR;    /* SPIx CRC Polynomial Register */
    volatile uint32_t RXCRCR;   /* SPIx RX CRC Register */
    volatile uint32_t TXCRCR;   /* SPIx TX CRC Register */
    volatile uint32_t I2SCFGR;  /* SPIx I2S Configuration Register */
    volatile uint32_t I2SPR;    /* SPIx I2S Prescaler Register */
} spi_t;

static inline spi_t *get_spi(int x) {
    switch(x) {
        case 1: return (spi_t *) SPI1_BASE;
        case 2: return (spi_t *) SPI2_BASE;
        case 3: return (spi_t *) SPI3_BASE;
        case 4: return (spi_t *) SPI4_BASE;
        case 5: return (spi_t *) SPI5_BASE;
        case 6: return (spi_t *) SPI6_BASE;
    }
    return (spi_t *) INVALID_PERIPH_BASE;
}

/* SPIx Control Register 1 bit fields */
#define SPI_CR1_CPHA                ((uint32_t) (1 << 0))   /* SPI_CR1 Clock phase */
#define SPI_CR1_CPOL                ((uint32_t) (1 << 1))   /* SPI_CR1 Clock polarity */
#define SPI_CR1_MSTR                ((uint32_t) (1 << 2))   /* SPI_CR1 Master selection */
#define SPI_CR1_BR_MASK             ((uint32_t) (0x38))     /* SPI_CR1 Baud rate mask */
#define SPI_CR1_BR_2                ((uint32_t) (0 << 3))   /* SPI_CR1 Baud rate = fPCLK/2 */
#define SPI_CR1_BR_4                ((uint32_t) (1 << 3))   /* SPI_CR1 Baud rate = fPCLK/4 */
#define SPI_CR1_BR_8                ((uint32_t) (2 << 3))   /* SPI_CR1 Baud rate = fPCLK/8 */
#define SPI_CR1_BR_16               ((uint32_t) (3 << 3))   /* SPI_CR1 Baud rate = fPCLK/16 */
#define SPI_CR1_BR_32               ((uint32_t) (4 << 3))   /* SPI_CR1 Baud rate = fPCLK/32 */
#define SPI_CR1_BR_64               ((uint32_t) (5 << 3))   /* SPI_CR1 Baud rate = fPCLK/64 */
#define SPI_CR1_BR_128              ((uint32_t) (6 << 3))   /* SPI_CR1 Baud rate = fPCLK/128 */
#define SPI_CR1_BR_256              ((uint32_t) (7 << 3))   /* SPI_CR1 Baud rate = fPCLK/256 */
#define SPI_CR1_SPE                 ((uint32_t) (1 << 6))   /* SPI_CR1 Enable */
#define SPI_CR1_LSBFIRST            ((uint32_t) (1 << 7))   /* SPI_CR1 LSB transmitted first */
#define SPI_CR1_SSI                 ((uint32_t) (1 << 8))   /* SPI_CR1 Internal slave select */
#define SPI_CR1_SSM                 ((uint32_t) (1 << 9))   /* SPI_CR1 Software slave management */
#define SPI_CR1_DFF                 ((uint32_t) (1 << 11))  /* SPI_CR1 Data frame format (0 = 8bit, 1 = 16bit) */
#define SPI_CR1_CRCNEXT             ((uint32_t) (1 << 12))  /* SPI_CR1 CRC transfer next */
#define SPI_CR1_CRCEN               ((uint32_t) (1 << 13))  /* SPI_CR1 Hardware CRC enable */
#define SPI_CR1_BIDIOE              ((uint32_t) (1 << 14))  /* SPI_CR1 Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE            ((uint32_t) (1 << 15))  /* SPI_CR1 bidirectional data mode enable */

/* SPIx Control Register 2 bit fields */
#define SPI_CR2_RXDMAEN             ((uint32_t) (1 << 0))   /* SPI_CR2 RX DMA enable */
#define SPI_CR2_TXDMAEN             ((uint32_t) (1 << 1))   /* SPI_CR2 TX DMA enable */
#define SPI_CR2_SSOE                ((uint32_t) (1 << 2))   /* SPI_CR2 SS output enable */
#define SPI_CR2_FRF                 ((uint32_t) (1 << 4))   /* SPI_CR2 Frame format */
#define SPI_CR2_FRF_MOTO            ((uint32_t) (0 << 4))   /* SPI_CR2 Frame format - Motorola */
#define SPI_CR2_FRF_TI              ((uint32_t) (1 << 4))   /* SPI_CR2 Frame format - TI */
#define SPI_CR2_ERRIE               ((uint32_t) (1 << 5))   /* SPI_CR2 Error interrupt enable */
#define SPI_CR2_RXNEIE              ((uint32_t) (1 << 6))   /* SPI_CR2 RX buffer not empty interrupt enable */
#define SPI_CR2_TXEIE               ((uint32_t) (1 << 7))   /* SPI_CR2 TX buffer empty interrupt enable */

/* SPIx Status Register bit fields */
#define SPI_SR_RXNE                 ((uint32_t) (1 << 0))   /* SPI_SR Receive not empty */
#define SPI_SR_TXNE                 ((uint32_t) (1 << 1))   /* SPI_SR Transmit not empty */
#define SPI_SR_CHSIDE               ((uint32_t) (1 << 2))   /* SPI_SR Channel side */
#define SPI_SR_UDR                  ((uint32_t) (1 << 3))   /* SPI_SR Underrun flag */
#define SPI_SR_CRCERR               ((uint32_t) (1 << 4))   /* SPI_SR CRC error flag */
#define SPI_SR_MODF                 ((uint32_t) (1 << 5))   /* SPI_SR Mode fault */
#define SPI_SR_OVR                  ((uint32_t) (1 << 6))   /* SPI_SR Overrun flag */
#define SPI_SR_BSY                  ((uint32_t) (1 << 7))   /* SPI_SR Busy flag */
#define SPI_SR_FRE                  ((uint32_t) (1 << 8))   /* SPI_SR Frame format error */

/* SPIx I2S Configuration Register bit fields */
#define SPI_I2SCFGR_CHLEN           ((uint32_t) (1 << 0))   /* SPI_I2SCFGR Channel length */
#define SPI_I2SCFGR_CHLEN_16        ((uint32_t) (0 << 0))   /* SPI_I2SCFGR Channel length - 16-bits */
#define SPI_I2SCFGR_CHLEN_32        ((uint32_t) (1 << 0))   /* SPI_I2SCFGR Channel length - 32-bits */

#define SPI_I2SCFGR_DATLEN_MASK     ((uint32_t) (3 << 1))   /* SPI_I2SCFGR Data length mask */
#define SPI_I2SCFGR_DATLEN_16       ((uint32_t) (0 << 1))   /* SPI_I2SCFGR Data length - 16-bits */
#define SPI_I2SCFGR_DATLEN_24       ((uint32_t) (1 << 1))   /* SPI_I2SCFGR Data length - 24-bits */
#define SPI_I2SCFGR_DATLEN_16       ((uint32_t) (2 << 1))   /* SPI_I2SCFGR Data length - 32-bits */

#define SPI_I2SCFGR_CKPOL           ((uint32_t) (1 << 3))   /* SPI_I2SCFGR Steady state clock polarity */
#define SPI_I2SCFGR_CKPOL_LOW       ((uint32_t) (0 << 3))   /* SPI_I2SCFGR Steady state clock polarity - low */
#define SPI_I2SCFGR_CKPOL_HIGH      ((uint32_t) (1 << 3))   /* SPI_I2SCFGR Steady state clock polarity - high */

#define SPI_I2SCFGR_I2SSTD_MASK     ((uint32_t) (3 << 4))   /* SPI_I2SCFGR I2S standard mask */
#define SPI_I2SCFGR_I2SSTD_PHL      ((uint32_t) (0 << 4))   /* SPI_I2SCFGR I2S standard - Phillips */
#define SPI_I2SCFGR_I2SSTD_MSB      ((uint32_t) (1 << 4))   /* SPI_I2SCFGR I2S standard - MSB justified */
#define SPI_I2SCFGR_I2SSTD_LSB      ((uint32_t) (2 << 4))   /* SPI_I2SCFGR I2S standard - LSB justified */
#define SPI_I2SCFGR_I2SSTD_PCM      ((uint32_t) (3 << 4))   /* SPI_I2SCFGR I2S standard - PCM */

#define SPI_I2SCFGR_PCMSYNC         ((uint32_t) (1 << 7))   /* SPI_I2SCFGR PCM frame synchronization */
#define SPI_I2SCFGR_PCMSYNC_SHRT    ((uint32_t) (0 << 7))   /* SPI_I2SCFGR PCM frame synchronization - short */
#define SPI_I2SCFGR_PCMSYNC_LONG    ((uint32_t) (1 << 7))   /* SPI_I2SCFGR PCM frame synchronization - long */

#define SPI_I2SCFGR_I2SCGR_MASK     ((uint32_t) (3 << 8))   /* SPI_I2SCFGR I2S configuraton mode mask */
#define SPI_I2SCFGR_I2SCGR_SLV_TX   ((uint32_t) (0 << 8))   /* SPI_I2SCFGR I2S configuraton mode - slave, transmit */
#define SPI_I2SCFGR_I2SCGR_SLV_RX   ((uint32_t) (1 << 8))   /* SPI_I2SCFGR I2S configuraton mode - slave, receive */
#define SPI_I2SCFGR_I2SCGR_MST_TX   ((uint32_t) (2 << 8))   /* SPI_I2SCFGR I2S configuraton mode - master, transmit */
#define SPI_I2SCFGR_I2SCGR_MST_RX   ((uint32_t) (3 << 8))   /* SPI_I2SCFGR I2S configuraton mode - master, receive */

#define SPI_I2SCFGR_I2SE            ((uint32_t) (1 << 10))  /* SPI_I2SCFGR I2S enable */

#define SPI_I2SCFGR_I2SMOD          ((uint32_t) (1 << 11))  /* SPI_I2SCFGR I2S mode */
#define SPI_I2SCFGR_I2SMOD_SPI      ((uint32_t) (0 << 11))  /* SPI_I2SCFGR I2S mode - SPI */
#define SPI_I2SCFGR_I2SMOD_I2S      ((uint32_t) (1 << 11))  /* SPI_I2SCFGR I2S mode - I2S */

/* SPIx I2S Prescaler Register bit fields */
#define SPI_I2SPR_I2SDIV_MASK       ((uint32_t) (0xff)      /* SPI_I2SPR I2C linear prescaler mask */
#define SPI_I2SPR_ODD               ((uint32_t) (1 << 8)    /* SPI_I2SPR Odd factor for prescaler */
#define SPI_I2SPR_MCKOE             ((uint32_t) (1 << 9)    /* SPI_I2SPR Master clock output enable */

#endif
