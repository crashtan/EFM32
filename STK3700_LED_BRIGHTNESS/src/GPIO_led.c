/*
 * GPIO_led.c
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include "em_cmu.h"
#include "GPIO_led.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "PWM.h"


void setupGPIO(void) {

	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure PB9 and PB10 as input */
	GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);
}

void GPIO_EN_buttons(void) {
	/* Set falling edge interrupt for both ports */
	GPIO_IntConfig(gpioPortB, 9, false, true, true);
	GPIO_IntConfig(gpioPortB, 10, false, true, true);

	/* Enable interrupt in core for even and odd gpio interrupts */
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void TIMER0_IRQHandler(void){
	//Clear Interrupt Request
	TIMER_IntClear(TIMER0, TIMER_IF_OF);
	milliCounter++;
}

//PB0 (PB9)
void GPIO_ODD_IRQHandler(void){
	GPIO_IntClear(1<<9);
	/* DECREASE BRIGHTNESS */
	if ( th_factor <= 100 )
		th_factor += 10;
}

//PB1 (PB10)
void GPIO_EVEN_IRQHandler(void) {
	GPIO_IntClear(1<<10);
	/* INCREASE BRIGHTNESS */
	if ( th_factor >= 10 )
		th_factor -=10;
}
