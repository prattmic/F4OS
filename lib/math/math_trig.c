#include <math.h>

// Fast arctan2 -- Courtesy of Jim Shima. Found on teh mighty interwebs
float atan2(float y, float x) {
    float coeff_1 = FLOAT_PI/4;
    float coeff_2 = 3*coeff_1;
    float abs_y = fabsf(y)+1e-10;      // kludge to prevent 0/0 condition
    float angle = 0.0f;
    if (x >= 0) {
        float r = (x - abs_y) / (x + abs_y);
        angle = coeff_1 - coeff_1 * r;
    }
    else {
        float r = (x + abs_y) / (abs_y - x);
        angle = coeff_2 - coeff_1 * r;
    }
    if (y < 0)
        return -angle;     // negate if in quad III or IV
    else
        return angle;
}
