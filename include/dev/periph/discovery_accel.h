#ifndef DEV_PERIPH_DISCOVERY_ACCEL_H_INCLUDED
#define DEV_PERIPH_DISCOVERY_ACCEL_H_INCLUDED

#define DISCOVERY_ACCEL_SENSITIVITY     (0.018)

rd_t open_discovery_accel(void) __attribute__((section(".kernel")));

#endif
