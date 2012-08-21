#include <stddef.h>
#include <math.h>

float lowpass(float acc, float new, float gain) {
    __asm__("\
            vmov.F32    s3, #1.0                    \r\n\
            vsub.F32    s3, s3, %[gain]             \r\n\
            vmul.F32    %[acc], %[acc], s3          \r\n\
            vfma.F32    %[acc], %[gain], %[new]     \r\n\
            "
            ::[acc] "w" (acc), [new] "w" (new), [gain] "w" (gain)
            :"cc", "memory", "s3");
    return acc;
}

