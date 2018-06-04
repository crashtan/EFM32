/*
 * SystemTick.h
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#ifndef SYSTEMTICK_H_
#define SYSTEMTICK_H_

#include <stdint.h>
#include "em_cmu.h"


void SetupSystemTick(void);
void SysTick_Handler(void);
void DelayInMS(uint32_t dlyTicks);

#endif /* SYSTEMTICK_H_ */
