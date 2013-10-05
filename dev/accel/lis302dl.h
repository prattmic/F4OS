#ifndef DEV_ACCEL_LIS302DL_H
#define DEV_ACCEL_LIS302DL_H

/* When this bit is set, the register address on the accelerometer
 * will automatically increment for multi-byte reads/writes */
#define ADDR_INC (1 << 6)

/* When set in register addres, perform a read instead of write */
#define SPI_READ    (1 << 7)

/* g/digit in raw data */
#define LIS302DL_SENSITIVITY     (0.018f)

/* Registers */
#define LIS302DL_WHOAMI     ((uint8_t) 0x0F)
#define LIS302DL_CTRL1      ((uint8_t) 0x20)
#define LIS302DL_CTRL2      ((uint8_t) 0x21)
#define LIS302DL_CTRL3      ((uint8_t) 0x22)
#define LIS302DL_HP_RST     ((uint8_t) 0x23)
#define LIS302DL_STATUS     ((uint8_t) 0x27)
#define LIS302DL_OUTX       ((uint8_t) 0x29)
#define LIS302DL_OUTY       ((uint8_t) 0x2B)
#define LIS302DL_OUTZ       ((uint8_t) 0x2D)

#define LIS302DL_CTRL1_XEN  ((uint8_t) (1 << 0))    /* X axis enable */
#define LIS302DL_CTRL1_YEN  ((uint8_t) (1 << 1))    /* Y axis enable */
#define LIS302DL_CTRL1_ZEN  ((uint8_t) (1 << 2))    /* Z axis enable */
#define LIS302DL_CTRL1_STM  ((uint8_t) (1 << 3))    /* Self-test M */
#define LIS302DL_CTRL1_STP  ((uint8_t) (1 << 4))    /* Self-test P */
#define LIS302DL_CTRL1_FS   ((uint8_t) (1 << 5))    /* Full-scale selection */
#define LIS302DL_CTRL1_PD   ((uint8_t) (1 << 6))    /* Power-down control (0=off) */
#define LIS302DL_CTRL1_DR   ((uint8_t) (1 << 7))    /* Data rate (0=100Hz, 1=400Hz) */

#endif
