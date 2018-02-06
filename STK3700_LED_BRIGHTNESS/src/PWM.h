/*
 * PWM.h
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include <stdint.h>

#ifndef SRC_PWM_H_
#define SRC_PWM_H_

extern volatile uint16_t milliCounter;
extern uint16_t th_factor;

void runLedPWM (void);

#endif /* SRC_PWM_H_ */
