/*
 * PWM.c
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include "PWM.h"
#include "GPIO_led.h"
#include "em_gpio.h"
#include "led.h"
#include "timer.h"

uint16_t PWM_value = 101;
volatile uint16_t milliCounter = 1;
uint16_t th_factor = 0;

void runLedPWM (void) {
	//Change brightness of left LED
	while (milliCounter <= PWM_value-th_factor) {}
	milliCounter = 0;
	turnOffLed0();

	while (milliCounter <= PWM_value+th_factor) {}
	milliCounter = 0;
	turnOnLed0();
}
