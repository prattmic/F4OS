/*
 * This file was derived from the Newlib open source project,
 * and is subject to its copyright notices and licenses, which
 * can be found in lib/math/newlib/LICENSE.
 */

#include <stdint.h>
#include <float.h>
#include <math.h>

static const float ROOT3 = 1.732050807f;
static const float a[] = { 0.0f, 0.523598775f, 1.570796326f,
                     1.047197551f };
static const float q[] = { 0.1412500740e+1f };
static const float p[] = { -0.4708325141f, -0.5090958253e-1f };

float atangentf(float x, float v, float u, int arctan2) {
    float f, g, R, P, Q, A, res;
    int N;
    int branch = 0;
    int expv, expu;

    /* Preparation for calculating arctan2. */
    if (arctan2) {
        if (u == 0.0f) {
            if (v == 0.0f) {
                return (FLOAT_NAN);
            }
            else {
                branch = 1;
                res = FLOAT_PI_OVER_TWO;
            }
        }

        if (!branch) {
            int e;
            /* Get the exponent values of the inputs. */
            g = frexpf (v, &expv);
            g = frexpf (u, &expu);

            /* See if a divide will overflow. */
            e = expv - expu;
            if (e > FLT_MAX_EXP) {
                branch = 1;
                res = FLOAT_PI_OVER_TWO;
            }

            /* Also check for underflow. */
            else if (e < FLT_MIN_EXP) {
                branch = 2;
                res = 0.0f;
            }
        }
    }

    if (!branch) {
        if (arctan2)
            f = fabsf (v / u);
        else
            f = fabsf (x);

        if (f > 1.0f) {
            f = 1.0f / f;
            N = 2;
        }
        else
            N = 0;

        if (f > (2.0f - ROOT3)) {
            A = ROOT3 - 1.0f;
            f = (((A * f - 0.5f) - 0.5f) + f) / (ROOT3 + f);
            N++;
        }

        /* Check for values that are too small. */
        if (-ROOTEPS < f && f < ROOTEPS)
            res = f;

        /* Calculate the Taylor series. */
        else {
            g = f * f;
            P = (p[1] * g + p[0]) * g;
            Q = g + q[0];
            R = P / Q;

            res = f + f * R;
        }

        if (N > 1)
            res = -res;

        res += a[N];
    }

    if (arctan2) {
        if (u < 0.0f)
            res = FLOAT_PI - res;
        if (v < 0.0f)
            res = -res;
    }
    else if (x < 0.0f) {
        res = -res;
    }

    return (res);
}

float atanf(float x) {
    switch (numtestf (x))
    {
        case NAN:
            return (x);
        case INF:
            /* this should check to see if neg NaN or pos NaN... */
            return (FLOAT_PI_OVER_TWO);
        case 0:
            return (0.0f);
        default:
            return (atangentf (x, 0, 0, 0));
    }
}
