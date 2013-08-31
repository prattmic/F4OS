#include <math.h>

float fabsf(float num) {
    __asm__("vabs.f32   %[num], %[num]  \r\n"
            :[num] "+w" (num)::"cc");

    return num;
}

float sqrtf(float num) {
    __asm__("vsqrt.f32   %[num], %[num]  \r\n"
            :[num] "+w" (num)::"cc");

    return num;
}
