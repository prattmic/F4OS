#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <stdint.h>

uint32_t pow(uint32_t base, uint32_t exp);

inline float fabsf(float num) __attribute__((always_inline));
inline float fabsf(float num) {
    __asm__("vabs.f32   %[num], %[num]  \r\n"
            :[num] "+w" (num)::"cc");

    return num;
}

inline float sqrtf(float num) __attribute__((always_inline));
inline float sqrtf(float num) {
    __asm__("vsqrt.f32   %[num], %[num]  \r\n"
            :[num] "+w" (num)::"cc");

    return num;
}

float logarithm(float x, int ten);
float frexpf (float d, int *exp);
uint8_t numtestf(float x);
float floorf(float x);
float modff(float x, float *iptr);
float powf(float x, float y);
float ldexpf(float d, int e);
float expf(float x);
float atan2(float y, float x);
float lowpass(float acc, float new, float gain);

#define FLOAT_INF           0x7f800000
#define FLOAT_NAN           0xffd00000

#define FLOAT_PI            3.14159265358979323846f
#define FLOAT_SQRT_HALF     0.70710678118654752440f
#define FLOAT_PI_OVER_TWO   1.57079632679489661923132f

#define INF                 1
#define NAN                 2
#define NUM                 3

#define BIGX                7.09782712893383973096e+02f
#define SMALLX              -7.45133219101941108420e+02f
#define ROOTEPS             1.7263349182589107e-4f
#define RAD_TO_DEG          180/FLOAT_PI
#define DEG_TO_RAD          FLOAT_PI/180

typedef union {
    float f;
    uint32_t i;
} float_int;

inline uint8_t isnan(float x) __attribute__((always_inline));
inline uint8_t isinf(float x) __attribute__((always_inline));
inline uint8_t isfinite(float x) __attribute__((always_inline));
inline uint8_t ispos(float x) __attribute__((always_inline));
inline uint32_t float_to_uint(float x) __attribute__((always_inline));
inline float uint_to_float(uint32_t x) __attribute__((always_inline));

inline uint8_t isnan(float x) {
    return (uint8_t) ((float_to_uint(x) & 0x7fffffff) > 0x7f800000);
}

inline uint8_t isinf(float x) {
    return (uint8_t) ((float_to_uint(x) & 0x7fffffff) == 0x7f800000);
}

inline uint8_t isfinite(float x) {
	return (uint8_t) ((uint32_t)((float_to_uint(x)&0x7fffffff)-0x7ff00000)>>31);
}

inline uint8_t ispos(float x) {
    return ((float_to_uint(x) & 0x80000000) ? 0 : 1);
}

inline uint32_t float_to_uint(float x) {
    float_int c;
    c.f = x;
    return c.i;
}

inline float uint_to_float(uint32_t x) {
    float_int c;
    c.i = x;
    return c.f;
}

#endif
