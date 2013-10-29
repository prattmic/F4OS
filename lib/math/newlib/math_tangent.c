/*
 * This file was derived from the Newlib open source project,
 * and is subject to its copyright notices and licenses, which
 * can be found in lib/math/newlib/LICENSE.
 */

#include <stdint.h>
#include <float.h>
#include <math.h>

static const float TWO_OVER_PI = 0.6366197723f;
static const float p[] = { -0.958017723e-1f };
static const float q[] = { -0.429135777f,
                            0.971685835e-2f };

float tanf(float x) {
    float y, f, g, xnum, xden, res;
    int N;

    /* Check for special values. */
    switch (numtestf (x)) {
        case NAN:
            return (x);
        case INF:
            return (FLOAT_NAN);
    }

    y = fabsf (x);

    /* Check for values that are out of our range. */
    if (y > 105414357.0f)
    {
        return (y);
    }

    if (x < 0.0f)
        N = (int) (x * TWO_OVER_PI - 0.5f);
    else
        N = (int) (x * TWO_OVER_PI + 0.5f);

    f = x - N * FLOAT_PI_OVER_TWO;

    /* Check for values that are too small. */
    if (-ROOTEPS < f && f < ROOTEPS)
    {
        xnum = f;
        xden = 1.0f;
    }

    /* Calculate the polynomial. */
    else
    {
        g = f * f;

        xnum = f * (p[0] * g) + f;
        xden = (q[1] * g + q[0]) * g + 1.0f;
    }

    /* Check for odd or even values. */
    if (N & 1)
    {
        xnum = -xnum;
        res = xden / xnum;
    }
    else
    {
        res = xnum / xden;
    }

    return (res);
}
