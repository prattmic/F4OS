/*
 * Portions of this file were derived from the Newlib open source project,
 * and are subject to its copyright notices and licenses, which
 * can be found in lib/math/newlib/LICENSE.
 */

#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <stdint.h>
#include <compiler.h>

uint32_t pow(uint32_t base, uint32_t exp);

float fabsf(float num);

float sqrtf(float num);
#define sqrt(x) sqrtf(x)

float logarithm(float x, int ten);
float frexpf (float d, int *exp);
uint8_t numtestf(float x);
float floorf(float x);
float modff(float x, float *iptr);
float powf(float x, float y);
float ldexpf(float d, int e);
float expf(float x);

float sinef(float x, int cosine);
#define sin(x)  sinef(x, 0)
#define cos(x)  sinef(x, 1)
float tanf(float x);
#define tan(x)  tanf(x)
float asinef(float x, int acosine);
#define asin(x)  asinef(x, 0)
#define acos(x)  asinef(x, 1)
float atangentf(float x, float v, float u, int arctan2);
#define atan2(v,u)  atangentf(0.0, v, u, 1)
float atanf(float x);
#define atan(x) atanf(x)

/*
 * First-order lowpass filter
 *
 * Performs a single iteration of a first-order low-pass filter
 *
 * @param acc   Accumulated value
 * @param new   New input
 * @param gain  Gain of low-pass filter
 * @returns New accumulated value
 */
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

static __always_inline uint32_t float_to_uint(float x) {
    float_int c;
    c.f = x;
    return c.i;
}

static __always_inline float uint_to_float(uint32_t x) {
    float_int c;
    c.i = x;
    return c.f;
}

static __always_inline uint8_t isnan(float x) {
    return (uint8_t) ((float_to_uint(x) & 0x7fffffff) > 0x7f800000);
}

static __always_inline uint8_t isinf(float x) {
    return (uint8_t) ((float_to_uint(x) & 0x7fffffff) == 0x7f800000);
}

static __always_inline uint8_t isfinite(float x) {
	return (uint8_t) ((uint32_t)((float_to_uint(x)&0x7fffffff)-0x7ff00000)>>31);
}

static __always_inline uint8_t ispos(float x) {
    return ((float_to_uint(x) & 0x80000000) ? 0 : 1);
}

#define DIV_ROUND_UP(dividend, divisor) \
    (((dividend) + (divisor) - 1) / (divisor))

#endif
