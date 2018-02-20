/*
 * PWM.c
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */

//PROGRAM
#include "PWM.h"

//EM_LIB
#include "em_gpio.h"

//GPIO_LIB
#include "GPIO_lib/led.h"
#include "GPIO_lib/timer.h"

uint16_t PWM_value = PWM_VALUE;
uint16_t th_factor = 0;

//milliCounter is in timer.h

void runLedPWM (void) {
	//Change brightness of left LED
	while (milliCounter <= PWM_value-th_factor) {}
	milliCounter = 0;
	turnOffLed0();

	while (milliCounter <= PWM_value+th_factor) {}
	milliCounter = 0;
	turnOnLed0();
}

void decreaseBrightness(void) {
/* DECREASE BRIGHTNESS */
	if ( th_factor <= 100 )
		th_factor += 10;
}

void increaseBrightness(void) {
	/* INCREASE BRIGHTNESS */
	if ( th_factor >= 10 )
		th_factor -=10;
}
