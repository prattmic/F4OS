#ifndef USR_FLIGHT_SENSORS_H_INCLUDED
#define USR_FLIGHT_SENSORS_H_INCLUDED

#include <dev/sensors.h>

struct sensors {
    struct accelerometer accel;
    struct gyro gyro;
    float temp;
    struct barometer baro;
    struct magnetometer mag;
};

extern struct sensors current_sensor_readings;
extern struct semaphore sensor_semaphore;

void init_sensors(void);

#endif
