/*
 * Utility.c
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#include "Utility.h"

void setBit(void* number, uint8_t bit_shift, uint8_t bit_value) {
	uint64_t* new_number = (uint64_t*)number;
	uint8_t new_bit_value = !!bit_value;    // Also booleanize to force 0 or 1
	*new_number ^= (-new_bit_value ^ *new_number) & (1UL << bit_shift);
}
