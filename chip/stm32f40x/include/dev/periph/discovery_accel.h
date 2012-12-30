#ifndef DEV_PERIPH_DISCOVERY_ACCEL_H_INCLUDED
#define DEV_PERIPH_DISCOVERY_ACCEL_H_INCLUDED

#define DISCOVERY_ACCEL_SENSITIVITY     (0.018f)

rd_t open_discovery_accel(void) __attribute__((section(".kernel")));
int read_discovery_accel(rd_t rd, struct accelerometer *accel);

#endif
