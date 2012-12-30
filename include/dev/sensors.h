#ifndef DEV_SENSORS_H_INCLUDED
#define DEV_SENSORS_H_INCLUDED

/* Basic data structures for sensors */

struct magnetometer {
    float x;
    float y;
    float z;
};

struct accelerometer {
    float x;
    float y;
    float z;
};

struct gyro {
    float x;
    float y;
    float z;
};

struct barometer {
    float pressure;
    float temp;
};

#endif
