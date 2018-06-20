/*
 * led.c
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include <stdbool.h>
#include "em_gpio.h"
#include "led.h"
#include "em_cmu.h"

/*******************************************************************************************/

void setupLEDs(void) {

CMU_ClockEnable(cmuClock_GPIO, true);

GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);	//Set LED0 as digital output
GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);	//Set LED1 as digital output

}

/*******************************************************************************************/

void turnOnLed0(void){
	GPIO_PinOutSet(gpioPortE, 2);
}

void turnOnLed1(void){
	GPIO_PinOutSet(gpioPortE, 3);
}
/*******************************************************************************************/

void turnOffLed0(void){
	GPIO_PinOutClear(gpioPortE, 2);
}

void turnOffLed1(void){
	GPIO_PinOutClear(gpioPortE, 3);
}
/*******************************************************************************************/

void toggleLed0(void){
	GPIO_PinOutToggle(gpioPortE, 2);
}

void toggleLed1(void){
	GPIO_PinOutToggle(gpioPortE, 3);
}
/*******************************************************************************************/

void turnOffLeds(void){
	/* Turn on both LEDS */
	GPIO_PinOutClear(gpioPortE, 3);
	GPIO_PinOutClear(gpioPortE, 2);
}

void turnOnLeds(void){
	/* Turn on both LEDS */
	GPIO_PinOutSet(gpioPortE, 3);
	GPIO_PinOutSet(gpioPortE, 2);
}

/*******************************************************************************************/

void toggleLeds(void){
	/* Turn on both LEDS */
	GPIO_PinOutSet(gpioPortE, 3);
	GPIO_PinOutSet(gpioPortE, 2);
}
/*******************************************************************************************/
