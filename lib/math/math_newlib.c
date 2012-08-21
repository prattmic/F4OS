/* This file was compiled from various components of the Newlib project.
 * Components released under various licenses and are copyright various authors.
 *
 * http://sourceware.org/cgi-bin/cvsweb.cgi/src/COPYING.NEWLIB?rev=1.31&content-type=text/x-cvsweb-markup&cvsroot=src
 * http://www.sourceware.org/newlib/   */

#include <stdint.h>
#include <float.h>
#include <math.h>

static const float a[] = { -0.5527074855 };
static const float b[] = { -0.6632718214e+1 };
static const float C1 = 0.693145752;
static const float C2 = 1.428606820e-06;
static const float C3 = 0.4342944819;

float logarithm(float x, int ten) {
    int N;
    float f, w, z;

    /* Check for domain/range errors here. */
    if (x == 0.0f) {
        return (-FLOAT_INF);
    }
    else if (x < 0.0f) {
        return (FLOAT_NAN);
    }
    else if (!isfinite(x)) {
        if (isnan(x)) {
            return (FLOAT_NAN);
        }
        else {
            return (FLOAT_INF);
        }
    }

    /* Get the exponent and mantissa where x = f * 2^N. */
    f = frexpf(x, &N);

    z = f - 0.5f;

    if (f > FLOAT_SQRT_HALF) {
        z = (z - 0.5f) / (f * 0.5f + 0.5f);
    }
    else {
        N--;
        z /= (z * 0.5f + 0.5f);
    }
    w = z * z;

    /* Use Newton's method with 4 terms. */
    z += z * w * (a[0]) / ((w + 1.0f) * w + b[0]);

    if (N != 0) {
        z = (N * C2 + z) + N * C1;
    }

    if (ten) {
        z *= C3;
    }

    return z;
}

/* Breaks fp number into mantissa and exponent */
float frexpf (float d, int *exp) {
    float f;
    int32_t wf, wd;

    /* Check for special values. */
    switch (numtestf(d)) {
        case NAN:
        case INF:
        case 0:
            *exp = 0;
            return (d);
    }

    wd = float_to_uint(d);

    /* Get the exponent. */
    *exp = ((wd & 0x7f800000) >> 23) - 126;

    /* Get the mantissa. */ 
    wf = wd & 0x7fffff;  
    wf |= 0x3f000000;

    f = uint_to_float(wf);

    return f;
}

uint8_t numtestf(float x) {
    int32_t wx;
    int exp;

    wx = float_to_uint(x);

    exp = (wx & 0x7f800000) >> 23;

    /* Check for a zero input. */
    if (x == 0.0f)
    {
        return 0;
    }

    /* Check for not a number or infinity. */
    if (exp == 0x7f8)
    {
        if(wx & 0x7fffff) {
            return NAN;
        }
        else {
            return INF;
        }
    }
     
    /* Otherwise it's a finite value. */ 
    else {
        return NUM;
    }
}

float floorf(float x) {
    float f, y;

    if (x > -1.0f && x < 1.0f) {
        return (x >= 0.0f ? 0.0f : -1.0f);
    }

    y = modff(x, &f);

    if (y == 0.0f) {
        return (x);
    }

    return (x >= 0.0f ? f : f - 1.0f);
}

static const float one = 1.0;

float modff(float x, float *iptr) {
	int32_t i0,j0;
	uint32_t i;

    i0 = float_to_uint(x);

	j0 = ((i0>>23)&0xff)-0x7f;	/* exponent of x */
	if(j0<23) {			/* integer part in x */
	    if(j0<0) {			/* |x|<1 */
	        *iptr = uint_to_float(i0&0x80000000);	/* *iptr = +-0 */
            return x;
	    }
        else {
            i = (0x007fffff)>>j0;
            if((i0&i)==0) {			/* x is integral */
                uint32_t ix;
                *iptr = x;
                ix = float_to_uint(x);
                x = uint_to_float(ix&0x80000000);	/* return +-0 */
                return x;
            } else {
                *iptr = uint_to_float(i0&(~i));
                return x - *iptr;
            }
	    }
	} else {			/* no fraction part */
	    uint32_t ix;
	    *iptr = x*one;
	    ix = float_to_uint(x);
	    x = uint_to_float(ix&0x80000000);	/* return +-0 */
	    return x;
	}
}

float powf(float x, float y) {
    float d, k, t, r = 1.0f;
    int n, sign, exponent_is_even_int = 0;
    int32_t px;

    px = float_to_uint(x);

    k = modff(y, &d);

    if (k == 0.0f) {
        /* Exponent y is an integer. */
        if (modff(ldexpf(y, -1), &t)) {
            /* y is odd. */
            exponent_is_even_int = 0;
        }
        else {
            /* y is even. */
            exponent_is_even_int = 1;
        }
    }

    if (x == 0.0f)
    {
        if (y <= 0.0f) {
            //errno = EDOM;
        }
    }
    else if ((t = y * logarithm(fabsf(x), 0)) >= BIGX) {
        if (px & 0x80000000) {
                /* x is negative. */
                if (k) {
                    /* y is not an integer. */
                    x = 0.0f;
                }
                else if (exponent_is_even_int) {
                  x = FLOAT_INF;
                }
                else {
                  x = -FLOAT_INF;
                }
        }
        else {
            x = FLOAT_INF;
        }
    }
    else if (t < SMALLX) {
        x = 0.0f;
    }
    else {
        if ( !k && fabsf(d) <= 32767.0f ) {
            n = (int) d;

            if ((sign = (n < 0))) {
                n = -n;
            }

            while ( n > 0 ) {
                if ((unsigned int) n % 2) {
                    r *= x;
                }
                x *= x;
                n = (unsigned int) n / 2;
            }

            if (sign) {
                r = 1.0f / r;
            }

            return r;
        }
        else {
            if ( px & 0x80000000 ) {
                /* x is negative. */
                if (k) {
                    /* y is not an integer. */
                    return 0.0f;
                }
            }

            x = expf(t);

            if (!exponent_is_even_int) { 
                if (px & 0x80000000) {
                    /* y is an odd integer, and x is negative,
                       so the result is negative. */
                    px = float_to_uint(x);
                    px |= 0x80000000;
                    x = uint_to_float(px);
                }
            }
        }
    }

    return x;
}

#define FLOAT_EXP_OFFS 127

float ldexpf(float d, int e) {
    int exp;
    int32_t wd;

    wd = float_to_uint(d);

    /* Check for special values and then scale d by e. */
    switch (numtestf(wd)) {
        case NAN:
            break;

        case INF:
            break;

        case 0:
            break;

        default:
            exp = (wd & 0x7f800000) >> 23;
            exp += e;

            if (exp > FLT_MAX_EXP + FLOAT_EXP_OFFS) {
                d = FLOAT_INF;
            }
            else if (exp < FLT_MIN_EXP + FLOAT_EXP_OFFS) {
                d = -FLOAT_INF;
            }
            else {
                wd &= 0x807fffff;
                wd |= exp << 23;
                d = uint_to_float(wd);
            }
      }

      return (d);
}

static const float INV_LN2 = 1.442695040;
static const float LN2 = 0.693147180;
static const float p[] = { 0.249999999950, 0.00416028863 };
static const float q[] = { 0.5, 0.04998717878 };

float expf(float x) {
    int N;
    float g, z, R, P, Q;

    switch (numtestf(x)) {
        case NAN:
            return (x);
        case INF:
            if (ispos(x))
                return (FLOAT_INF);
            else
                return (0.0f);
        case 0:
            return (1.0f);
    }

    /* Check for out of bounds. */
    if (x > BIGX || x < SMALLX) {
        return (x);
    }

    /* Check for a value too small to calculate. */
    if (-ROOTEPS < x && x < ROOTEPS) {
        return (1.0f);
    }

    /* Calculate the exponent. */
    if (x < 0.0f) {
        N = (int) (x * INV_LN2 - 0.5f);
    }
    else {
        N = (int) (x * INV_LN2 + 0.5f);
    }

    /* Construct the mantissa. */
    g = x - N * LN2;
    z = g * g;
    P = g * (p[1] * z + p[0]);
    Q = q[1] * z + q[0];
    R = 0.5f + P / (Q - P);

    /* Return the floating point value. */
    N++;
    return ldexpf(R, N);
}
