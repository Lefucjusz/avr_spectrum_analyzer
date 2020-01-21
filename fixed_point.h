/*
 * fixed_point.h
 *
 *  Created on: 17 sty 2020
 *      Author: Badziew
 */

#ifndef FIXED_POINT_H_
#define FIXED_POINT_H_

#include <stdint.h>

#define FP_M_BITS 7
#define FP_N_BITS 8
#define FP_ONE_HALF (1<<(FP_N_BITS - 1))
#define FP_ONE (1<<FP_N_BITS)

#define FP_LONG_M_BITS (FP_M_BITS<<1)
#define FP_LONG_N_BITS (FP_N_BITS<<1)
#define FP_LONG_ONE_HALF (1UL<<(FP_LONG_N_BITS - 1))
#define FP_LONG_ONE (1UL<<FP_LONG_N_BITS)

typedef int16_t fp_t;
typedef int32_t fp_long_t;

fp_t fp_add(fp_t, fp_t);
fp_t fp_sub(fp_t, fp_t);
fp_t fp_mul(fp_t, fp_t);
fp_t fp_div(fp_t, fp_t);
fp_t fp_abs(fp_t);
//fp_t fp_sqrt(fp_t);

#endif /* FIXED_POINT_H_ */
