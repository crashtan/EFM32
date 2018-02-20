/*
 * timer.h
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include <stdint.h>
#include <stdbool.h>
#include "em_timer.h"

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

extern volatile uint16_t milliCounter;

enum enumTimer {
	TIMER_0,
	TIMER_1,
	TIMER_2,
};

enum enumFrequency {
	MHZ_1,
	MHZ_11,
	MHZ_28,
};

enum enumPrescale {
	PSCALE_1,
	PSCALE_128,
	PSCALE_1024,
};

TIMER_Init_TypeDef timer_type_local;

// In order of importance to run -->
void setTimerType (enum enumPrescale prescale, enum enumFrequency frequency);
void setupTimers(enum enumTimer timer, uint32_t max_value);
void INTR_EN_timer(enum enumTimer timer);
void startTimer(enum enumTimer timer);
void delayInMS (uint16_t delay_time);
void TIMER0_IRQHandler(void);


#endif /* SRC_TIMER_H_ */
