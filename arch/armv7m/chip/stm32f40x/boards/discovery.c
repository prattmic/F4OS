/*
 * Copyright (C) 2013 F4OS Authors
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

#include <arch/chip/gpio.h>
#include <dev/hw/led.h>
#include <board_config.h>

struct lis302dl_accel_config lis302dl_accel_config = {
    .valid = BOARD_CONFIG_VALID_MAGIC,
    .parent_name = "spi1",
    .cs_gpio = STM32F4_GPIO_PE3,
    .cs_active_low = 0,
};

struct itg3200_gyro_config itg3200_gyro_config = {
    .valid = BOARD_CONFIG_VALID_MAGIC,
    .parent_name = "i2c1",
};

/* There is no hmc5883, so empty config */
struct hmc5883_mag_config hmc5883_mag_config;

/* There is no ms5611, so empty config */
struct ms5611_baro_config ms5611_baro_config;

/* There is no mpu6000, so empty config */
struct mpu6000_spi_config mpu6000_spi_config;

/* GPIO LEDs available - Red, Green, Orange, Blue */
const struct led leds_avail[] = {{STM32F4_GPIO_PD14, 0}, {STM32F4_GPIO_PD12, 0},
                                 {STM32F4_GPIO_PD13, 0}, {STM32F4_GPIO_PD15, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
