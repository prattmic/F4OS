/*
 * kalman.h
 *
 * Code generation for function 'kalman'
 *
 * C source code generated on: Wed Jan  2 16:01:01 2013
 *
 */

#ifndef __KALMAN_H__
#define __KALMAN_H__
/* Include files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"

#include "rtwtypes.h"
#include "kalman_types.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern void kalman(const state u, boolean_T measurement_update, const real_T y[3], real_T N, real_T T_out, const real_T R[9], const real_T Q[4], real_T ret[2]);
#endif
/* End of code generation (kalman.h) */
