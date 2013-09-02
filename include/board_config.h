#ifndef BOARD_CONFIG_H_INCLUDED
#define BOARD_CONFIG_H_INCLUDED

#include <stdint.h>

/* Temporary workaround for board configuration until we have device trees */

#define BOARD_CONFIG_VALID_MAGIC 0xdeb1afea

struct lis302dl_accel_config {
    uint32_t valid;
    char *parent_name;
    uint32_t cs_gpio;
    uint8_t cs_active_low;
};

extern struct lis302dl_accel_config lis302dl_accel_config;

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

#endif
