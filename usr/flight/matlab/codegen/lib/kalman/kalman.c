/*
 * kalman.c
 *
 * Code generation for function 'kalman'
 *
 * C source code generated on: Wed Jan  2 16:01:01 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "kalman.h"
#include "inv.h"
#include "kalman_data.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
static real_T rt_powd_snf(real_T u0, real_T u1);

/* Function Definitions */
static real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T d0;
  real_T d1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else {
    d0 = fabs(u0);
    d1 = fabs(u1);
    if (rtIsInf(u1)) {
      if (d0 == 1.0) {
        y = rtNaN;
      } else if (d0 > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d1 == 0.0) {
      y = 1.0;
    } else if (d1 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = rtNaN;
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

void kalman(const state u, boolean_T measurement_update, const real_T y[3],
            real_T N, real_T T_out, const real_T R[9], const real_T Q[4], real_T
            ret[2])
{
  int32_T i;
  real_T b_y;
  int32_T b_i;
  real_T A[4];
  real_T I[4];
  real_T dv0[4];
  int32_T i0;
  int32_T i1;
  real_T C[6];
  real_T c_y[6];
  real_T b_C[6];
  real_T a[9];
  real_T c[9];
  real_T b_c[9];
  real_T L[6];
  int8_T b_I[4];
  real_T b_ret[3];
  real_T d_y[3];

  /* KALMAN Preforms one step of pitch/roll Kalman filter */
  /*  x_hat is the roll/pitch estimate, u is all other state variables */
  /*  N is number of times to run prediction step */
  /*  T_out is output sample rate */
  /*  R is measurement noise covariance (3x3) */
  /*  Q is process noise covariance (2x2) */
  for (i = 0; i <= (int32_T)N - 1; i++) {
    b_y = T_out / N;
    for (b_i = 0; b_i < 2; b_i++) {
      ret[b_i] = 0.0;
    }

    ret[0] = (u.p + u.q * (real32_T)sin(x_hat[0]) * (real32_T)tan(x_hat[1])) +
      u.r * (real32_T)cos(x_hat[0]) * (real32_T)tan(x_hat[1]);
    ret[1] = u.q * (real32_T)cos(x_hat[0]) - u.r * (real32_T)sin(x_hat[1]);
    for (b_i = 0; b_i < 2; b_i++) {
      x_hat[b_i] += b_y * ret[b_i];
    }

    for (b_i = 0; b_i < 4; b_i++) {
      A[b_i] = 0.0;
    }

    A[0] = u.p * (real32_T)cos(x_hat[0]) * (real32_T)tan(x_hat[1]) - u.r *
      (real32_T)sin(x_hat[0]) * (real32_T)tan(x_hat[1]);
    A[2] = (u.q * (real32_T)sin(x_hat[0]) - u.r * (real32_T)cos(x_hat[0])) /
      (real32_T)rt_powd_snf(cos(x_hat[1]), 2.0);
    A[1] = -u.q * (real32_T)sin(x_hat[0]) - u.r * (real32_T)cos(x_hat[0]);
    A[3] = 0.0;
    b_y = T_out / N;
    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 2; i0++) {
        I[b_i + (i0 << 1)] = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          I[b_i + (i0 << 1)] += A[b_i + (i1 << 1)] * P[i1 + (i0 << 1)];
        }

        dv0[b_i + (i0 << 1)] = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          dv0[b_i + (i0 << 1)] += P[b_i + (i1 << 1)] * A[i0 + (i1 << 1)];
        }
      }
    }

    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 2; i0++) {
        P[i0 + (b_i << 1)] += b_y * ((I[i0 + (b_i << 1)] + dv0[i0 + (b_i << 1)])
          + Q[i0 + (b_i << 1)]);
      }
    }
  }

  if (measurement_update) {
    for (b_i = 0; b_i < 6; b_i++) {
      C[b_i] = 0.0;
    }

    C[0] = 0.0;
    C[3] = u.q * u.Va * (real32_T)cos(x_hat[1]) + u.g * (real32_T)cos(x_hat[1]);
    C[1] = -u.g * (real32_T)cos(x_hat[0]) * (real32_T)cos(x_hat[1]);
    C[4] = (-u.r * u.Va * (real32_T)sin(x_hat[1]) - u.p * u.Va * (real32_T)cos
            (x_hat[1])) + u.g * (real32_T)sin(x_hat[0]) * (real32_T)sin(x_hat[1]);
    C[2] = u.g * (real32_T)sin(x_hat[1]) * (real32_T)cos(x_hat[1]);
    C[5] = (u.q * u.Va + u.g * (real32_T)cos(x_hat[0])) * (real32_T)sin(x_hat[1]);
    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 3; i0++) {
        c_y[b_i + (i0 << 1)] = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          c_y[b_i + (i0 << 1)] += P[b_i + (i1 << 1)] * C[i0 + 3 * i1];
        }
      }
    }

    for (b_i = 0; b_i < 3; b_i++) {
      for (i0 = 0; i0 < 2; i0++) {
        b_C[b_i + 3 * i0] = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          b_C[b_i + 3 * i0] += C[b_i + 3 * i1] * P[i1 + (i0 << 1)];
        }
      }
    }

    for (b_i = 0; b_i < 3; b_i++) {
      for (i0 = 0; i0 < 3; i0++) {
        b_y = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          b_y += b_C[b_i + 3 * i1] * C[i0 + 3 * i1];
        }

        a[b_i + 3 * i0] = R[b_i + 3 * i0] + b_y;
      }
    }

    memcpy(&c[0], &a[0], 9U * sizeof(real_T));
    memcpy(&b_c[0], &c[0], 9U * sizeof(real_T));
    inv(b_c, c);
    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 3; i0++) {
        L[b_i + (i0 << 1)] = 0.0;
        for (i1 = 0; i1 < 3; i1++) {
          L[b_i + (i0 << 1)] += c_y[b_i + (i1 << 1)] * c[i1 + 3 * i0];
        }
      }
    }

    for (b_i = 0; b_i < 4; b_i++) {
      b_I[b_i] = 0;
    }

    for (i = 0; i < 2; i++) {
      b_I[i + (i << 1)] = 1;
    }

    for (b_i = 0; b_i < 4; b_i++) {
      A[b_i] = P[b_i];
    }

    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 2; i0++) {
        b_y = 0.0;
        for (i1 = 0; i1 < 3; i1++) {
          b_y += L[b_i + (i1 << 1)] * C[i1 + 3 * i0];
        }

        I[b_i + (i0 << 1)] = (real_T)b_I[b_i + (i0 << 1)] - b_y;
      }
    }

    for (b_i = 0; b_i < 2; b_i++) {
      for (i0 = 0; i0 < 2; i0++) {
        P[b_i + (i0 << 1)] = 0.0;
        for (i1 = 0; i1 < 2; i1++) {
          P[b_i + (i0 << 1)] += A[b_i + (i1 << 1)] * I[i1 + (i0 << 1)];
        }
      }
    }

    for (i = 0; i < 3; i++) {
      b_ret[i] = 0.0;
    }

    b_ret[0] = u.q * u.Va * (real32_T)sin(x_hat[1]) + u.g * (real32_T)sin(x_hat
      [1]);
    b_ret[1] = (u.r * u.Va * (real32_T)cos(x_hat[1]) - u.p * u.Va * (real32_T)
                sin(x_hat[1])) - u.g * (real32_T)cos(x_hat[1]) * (real32_T)sin
      (x_hat[0]);
    b_ret[2] = -u.q * u.Va * (real32_T)cos(x_hat[1]) - u.g * (real32_T)cos
      (x_hat[1]) * (real32_T)cos(x_hat[0]);
    for (b_i = 0; b_i < 3; b_i++) {
      d_y[b_i] = y[b_i] - b_ret[b_i];
    }

    for (b_i = 0; b_i < 2; b_i++) {
      b_y = 0.0;
      for (i0 = 0; i0 < 3; i0++) {
        b_y += L[b_i + (i0 << 1)] * d_y[i0];
      }

      x_hat[b_i] += b_y;
    }
  }

  for (i = 0; i < 2; i++) {
    ret[i] = x_hat[i];
  }
}

/* End of code generation (kalman.c) */
