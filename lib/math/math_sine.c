/* This file was compiled from various components of the Newlib project.
 * Components released under various licenses and are copyright their respective authors.
 *
 * http://sourceware.org/cgi-bin/cvsweb.cgi/src/COPYING.NEWLIB?rev=1.31&content-type=text/x-cvsweb-markup&cvsroot=src
 * http://www.sourceware.org/newlib/   */

#include <stdint.h>
#include <float.h>
#include <math.h>

static const float HALF_PI = 1.570796326f;
static const float ONE_OVER_PI = 0.318309886f;
static const float r[] = { -0.1666665668f,
                            0.8333025139e-02f,
                           -0.1980741872e-03f,
                            0.2601903036e-5f };

float sinef(float x, int cosine) {
    int sgn, N;
    float y, XN, g, R, res;
    float YMAX = 210828714.0f;

    switch (numtestf (x)) {
        case NAN:
            return (x);
        case INF:
            return (FLOAT_NAN);
    }

    /* Use sin and cos properties to ease computations. */
    if (cosine) {
        sgn = 1;
        y = fabsf (x) + HALF_PI;
    }
    else {
        if (x < 0.0) {
            sgn = -1;
            y = -x;
        }
        else {
            sgn = 1;
            y = x;
        }
    }

    /* Check for values of y that will overflow here. */
    if (y > YMAX) {
        return (x);
    }

    /* Calculate the exponent. */
    if (y < 0.0)
        N = (int) (y * ONE_OVER_PI - 0.5);
    else
        N = (int) (y * ONE_OVER_PI + 0.5);
    XN = (float) N;

    if (N & 1)
        sgn = -sgn;

    if (cosine)
        XN -= 0.5;

    y = fabsf (x) - XN * FLOAT_PI;

    if (-ROOTEPS < y && y < ROOTEPS)
        res = y;

    else {
        g = y * y;

        /* Calculate the Taylor series. */
        R = (((r[3] * g + r[2]) * g + r[1]) * g + r[0]) * g;

        /* Finally, compute the result. */
        res = y + y * R;
    }

    res *= sgn;

    return (res);
}

static const float p[] = { 0.933935835, -0.504400557 };
static const float q[] = { 0.560363004e+1, -0.554846723e+1 };
static const float a[] = { 0.0, 0.785398163 };
static const float b[] = { 1.570796326, 0.785398163 };

float asinef(float x, int acosine) {
    int flag, i;
    int branch = 0;
    float R, P, Q, y;
    float g = 0.0;
    float res = 0.0;

    /* Check for special values. */
    i = numtestf (x);
    if (i == NAN || i == INF) {
        if (i == NAN)
            return (x);
        else
            return (FLOAT_INF);
    }

    y = fabsf (x);
    flag = acosine;

    if (y > 0.5) {
        i = 1 - flag;

        /* Check for range error. */
        if (y > 1.0) {
            return (FLOAT_NAN);
        }

        g = (1 - y) / 2.0;
        y = -2 * sqrt (g);
        branch = 1;
    }
    else {
        i = flag;
        if (y < ROOTEPS)
            res = y;
        else
            g = y * y;
    }

    if (y >= ROOTEPS || branch == 1) {
        /* Calculate the Taylor series. */
        P = (p[1] * g + p[0]) * g;
        Q = (g + q[1]) * g + q[0];
        R = P / Q;

        res = y + y * R;
    }

    /* Calculate asine or acose. */
    if (flag == 0) {
        res = (a[i] + res) + a[i];
        if (x < 0.0)
            res = -res;
    }
    else {
        if (x < 0.0)
            res = (b[i] + res) + b[i];
        else
            res = (a[i] - res) + a[i];
    }

    return (res);
}
