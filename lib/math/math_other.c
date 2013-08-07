#include <stddef.h>
#include <math.h>

float lowpass(float acc, float new, float gain) {
    acc = acc * (1-gain);

    return acc + new * gain;
}
