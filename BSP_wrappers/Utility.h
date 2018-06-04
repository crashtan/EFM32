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
void setBit(void* number, uint8_t bit_shift, uint8_t bit_value);

#endif /* UTILITY_H_ */
