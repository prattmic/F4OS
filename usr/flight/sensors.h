#ifndef USR_FLIGHT_SENSORS_H_INCLUDED
#define USR_FLIGHT_SENSORS_H_INCLUDED

#include <dev/sensors.h>

struct sensors {
    struct accelerometer accel;
    struct gyro gyro;
    float temp;
    struct barometer baro;
    struct magnetometer mag;
    uint8_t new_accel : 1;
    uint8_t new_gyro : 1;
    uint8_t new_temp : 1;
    uint8_t new_baro : 1;
    uint8_t new_mag : 1;
};

extern struct sensors current_sensor_readings;
extern struct semaphore sensor_semaphore;

void init_sensors(void);

#endif
