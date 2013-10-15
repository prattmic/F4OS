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

#ifndef DEV_MPU6000_REGS_H_INCLUDED
#define DEV_MPU6000_REGS_H_INCLUDED

// MPU 6000 registers
#define MPU6000_SMPLRT_DIV                  0x19
#define MPU6000_CONFIG                      0x1A
#define MPU6000_GYRO_CONFIG                 0x1B
#define MPU6000_ACCEL_CONFIG                0x1C
#define MPU6000_ACCEL_XOUT_H                0x3B
#define MPU6000_ACCEL_XOUT_L                0x3C
#define MPU6000_ACCEL_YOUT_H                0x3D
#define MPU6000_ACCEL_YOUT_L                0x3E
#define MPU6000_ACCEL_ZOUT_H                0x3F
#define MPU6000_ACCEL_ZOUT_L                0x40
#define MPU6000_TEMP_OUT_H                  0x41
#define MPU6000_TEMP_OUT_L                  0x42
#define MPU6000_GYRO_XOUT_H                 0x43
#define MPU6000_GYRO_XOUT_L                 0x44
#define MPU6000_GYRO_YOUT_H                 0x45
#define MPU6000_GYRO_YOUT_L                 0x46
#define MPU6000_GYRO_ZOUT_H                 0x47
#define MPU6000_GYRO_ZOUT_L                 0x48
#define MPU6000_USER_CTRL                   0x6A
#define MPU6000_PWR_MGMT_1                  0x6B
#define MPU6000_PWR_MGMT_2                  0x6C
#define MPU6000_WHOAMI                      0x75

#define MPU6000_CONFIG_LPF_256HZ            0x00
#define MPU6000_CONFIG_LPF_190HZ            0x01
#define MPU6000_CONFIG_LPF_100HZ            0x02
#define MPU6000_CONFIG_LPF_50HZ             0x03
#define MPU6000_CONFIG_LPF_20HZ             0x04
#define MPU6000_CONFIG_LPF_10HZ             0x05
#define MPU6000_CONFIG_LPF_5HZ              0x06

#define MPU6000_GYRO_CONFIG_250DPS          0x00
#define MPU6000_GYRO_CONFIG_500DPS          0x08
#define MPU6000_GYRO_CONFIG_1000DPS         0x10
#define MPU6000_GYRO_CONFIG_2000DPS         0x18
#define MPU6000_GYRO_CONFIG_DPS_MASK        0x18

#define MPU6000_ACCEL_CONFIG_2G             0x00
#define MPU6000_ACCEL_CONFIG_4G             0x08
#define MPU6000_ACCEL_CONFIG_8G             0x10
#define MPU6000_ACCEL_CONFIG_16G            0x18
#define MPU6000_ACCEL_CONFIG_G_MASK         0x18

#define MPU6000_ACCEL_SENSITIVITY_2G        (16384.0f)
#define MPU6000_ACCEL_SENSITIVITY_4G        (8192.0f)
#define MPU6000_ACCEL_SENSITIVITY_8G        (4096.0f)
#define MPU6000_ACCEL_SENSITIVITY_16G       (2048.0f)

#define MPU6000_GYRO_SENSITIVITY_250DPS     (131.0f)
#define MPU6000_GYRO_SENSITIVITY_500DPS     (65.5f)
#define MPU6000_GYRO_SENSITIVITY_1000DPS    (32.8f)
#define MPU6000_GYRO_SENSITIVITY_2000DPS    (16.4f)

#define MPU6000_PWR_MGMT_1_SLEEP            0x40
#define MPU6000_PWR_MGMT_1_CLK_PLLGYROX     0x01

#define MPU6000_PWR_MGMT_2_STBY_XA          0x20
#define MPU6000_PWR_MGMT_2_STBY_YA          0x10
#define MPU6000_PWR_MGMT_2_STBY_ZA          0x08
#define MPU6000_PWR_MGMT_2_STBY_XG          0x04
#define MPU6000_PWR_MGMT_2_STBY_YG          0x02
#define MPU6000_PWR_MGMT_2_STBY_ZG          0x01

#endif
