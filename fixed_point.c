/*
 * fixed_point.c
 *
 *  Created on: 17 sty 2020
 *      Author: Badziew
 */

#include "fixed_point.h"

fp_t fp_add(fp_t a, fp_t b)
{
	return a+b;
}

fp_t fp_sub(fp_t a, fp_t b)
{
	return a-b;
}

fp_t fp_mul(fp_t a, fp_t b)
{
	fp_long_t temp;
	temp = (fp_long_t)a * (fp_long_t)b; //multiply
	temp += FP_ONE_HALF; //add 0.5 for rounding purposes
	temp >>= FP_N_BITS; //normalize by dividing by base
	return temp;
}

fp_t fp_div(fp_t a, fp_t b)
{
	fp_long_t temp;
	temp = (fp_long_t)a;
	temp <<= FP_N_BITS;
	temp /= (fp_long_t)b;
	return temp;
}

fp_t fp_abs(fp_t x)
{
	if(x < 0)
		return -x;
	return x;
}

//fp_t fp_sqrt(fp_t a) //using Babilonian approximation
//{
//	fp_t xn = FP_ONE; //some starting point
//	for(uint8_t i = 0; i < 5; i++) //more iterations give better precision, but slow down algorithm
//	{
//		xn = (xn + fp_div(a, xn)) >> 1;
//	}
//	return xn;
//}
