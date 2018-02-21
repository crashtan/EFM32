/*
 * buttons.c
 *
 *  Created on: Feb 13, 2018
 *      Author: Administrator
 */

/*******************************************************************************************/

//EXTERNAL
#include <stdbool.h>

//EM_LIB
#include "em_gpio.h"
#include "em_cmu.h"

//HEADER
#include "buttons.h"

/*******************************************************************************************/

void GPIO_EN_buttons(void) {

	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure PB9 and PB10 as input */
	GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);
}

/*******************************************************************************************/

void GPIO_EN_Intr(void) {
	/* Set falling edge interrupt for both ports */
	GPIO_IntConfig(gpioPortB, 9, false, true, true);
	GPIO_IntConfig(gpioPortB, 10, false, true, true);

	/* Enable interrupt in core for even and odd gpio interrupts */
	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/*******************************************************************************************/

//PB0 (PB9 in interrupt)
void GPIO_ODD_IRQHandler(void){
	GPIO_IntClear(1<<9);
	//Write button function here
}

/*******************************************************************************************/

//PB1 (PB10 in interrupt)
void GPIO_EVEN_IRQHandler(void) {
	GPIO_IntClear(1<<10);
	//Write button function here
}

/*******************************************************************************************/
