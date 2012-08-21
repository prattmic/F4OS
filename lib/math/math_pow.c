#include <stdint.h>
#include <math.h>

uint32_t pow(uint32_t base, uint32_t exp) {
    int result = base;

    if (exp == 0) {
        return 1;
    }

    exp--;
    while (exp) {
        result = result * base;
        exp--;
    }

    return result;
}
