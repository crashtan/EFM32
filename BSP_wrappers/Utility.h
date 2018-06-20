/*
 * Utility.h
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdint.h>

//*************
// Fixed Point
//*************
/* FIXED POINT EXAMPLE
 *  fx_t a, b, c, d, add, sub, mul, div;
 *
 *  a = fx_make(1.0f);
 *  b = fx_make(2.0f);
 *  c = fx_make(3.0f);
 *  d = fx_make(-2023.621f);
 *  printf("a=%f, b=%f, c=%f, d=%f\n", fx_float(a),fx_float(b),fx_float(c),fx_float(d));
 *
 *  add = fx_add(d, b);
 *  sub = fx_sub(a, c);
 *  mul = fx_mul(d, c);
 *  div = fx_div(a, c);
 *
 *  FX_FRAC=1 (Q31.1): min=-1.07374e+09, max=1.07374e+09, step=0.5
 *  FX_FRAC=8 (Q24.8): min=-8.38861e+06, max=8.38861e+06, step=0.00390625
 *  FX_FRAC=16 (Q16.16): min=-32768, max=32768, step=1.52588e-05
 *  FX_FRAC=24 (Q8.24): min=-128, max=128, step=5.96046e-08
 *  FX_FRAC=31 (Q1.31): min=1, max=-1, step=-4.65661e-10
 */

typedef int32_t fx_t;
#define FX_FRAC 16
#define fx_float(a) (a / (float)(1<<FX_FRAC))
#define fx_make(a)  ((fx_t)(a * (1<<FX_FRAC)))
#define fx_add(a,b) (a + b)
#define fx_sub(a,b) (a - b)
#define fx_mul(a,b) ((fx_t)(((int64_t)a * b) >> FX_FRAC))
#define fx_div(a,b) ((fx_t)(((int64_t)a << FX_FRAC) / b))

//*************
// Utilities
//*************
void setBit(uint8_t* number, uint8_t bit_shift, uint8_t bit_value);

#endif /* UTILITY_H_ */
