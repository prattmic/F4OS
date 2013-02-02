/*
 * kalman_initialize.c
 *
 * Code generation for function 'kalman_initialize'
 *
 * C source code generated on: Wed Jan  2 16:01:01 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "kalman.h"
#include "kalman_initialize.h"
#include "kalman_data.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void kalman_initialize(void)
{
  int32_T i;
  rt_InitInfAndNaN(8U);
  for (i = 0; i < 2; i++) {
    x_hat[i] = 0.0;
  }

  for (i = 0; i < 4; i++) {
    P[i] = 0.0;
  }
}

/* End of code generation (kalman_initialize.c) */
