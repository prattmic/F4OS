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

#ifndef BOARD_CONFIG_H_INCLUDED
#define BOARD_CONFIG_H_INCLUDED

#include <stdint.h>

/* Temporary workaround for board configuration until we have device trees */

#define BOARD_CONFIG_VALID_MAGIC 0xdeb1afea

struct itg3200_gyro_config {
    uint32_t valid;
    char *parent_name;
};

extern struct itg3200_gyro_config itg3200_gyro_config;

struct hmc5883_mag_config {
    uint32_t valid;
    char *parent_name;
};

extern struct hmc5883_mag_config hmc5883_mag_config;

struct ms5611_baro_config {
    uint32_t valid;
    char *parent_name;
};

extern struct ms5611_baro_config ms5611_baro_config;

struct mpu6000_spi_config {
    uint32_t valid;
    char *parent_name;
    uint32_t cs_gpio;
    uint8_t cs_active_low;
};

extern struct mpu6000_spi_config mpu6000_spi_config;

#ifdef CONFIG_AS5048B
struct as5048b_rotary_encoder_config {
    uint32_t valid;
    char *parent_name;
};

extern struct as5048b_rotary_encoder_config as5048b_rotary_encoder_config;
#endif

#endif
