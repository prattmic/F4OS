#ifndef USR_FLIGHT_FLIGHT_H_INCLUDED
#define USR_FLIGHT_FLIGHT_H_INCLUDED

struct state {
    float p;
    float q;
    float r;
    float theta;
    float phi;
    float Va;
    float g;
};

typedef struct state state;

#endif
