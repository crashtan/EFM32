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


void SystemTick_Setup(void);
void SysTick_Handler(void);
void DelayInMS(uint32_t dlyTicks);
uint32_t GetMilliseconds(void);

#endif /* SYSTEMTICK_H_ */
