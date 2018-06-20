/*
 * GPIO.c
 *
 *  Created on: Jun 7, 2018
 *      Author: moiz.hussain
 */
#include "GPIO.h"
#include "../ISL_29125/ISL_29125.h"
#include "SystemTick.h"

/**************************************************************************//**
 * @brief  Gpio callback
 * @param  pin - pin which triggered interrupt
 *****************************************************************************/
void gpioCallback(uint8_t pin)
{
	switch(pin) {
		//Interrupt Pin from ISL29125
		case 7:
			ISL29125_OnInterrupt();
			break;

		//Buttons
		#ifdef DEBUG_LOCAL
		case 9:	//PB0 on board
			CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_1);
			SystemTick_Setup();
			break;
		case 10: //PB1 on board
			CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_4);
			SystemTick_Setup();
			break;
		#endif

		//Other GPIO pins
		default:
			break;
	}
}

/**************************************************************************//**
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 *****************************************************************************/
void gpio_Setup(void)
{
	/* Enable GPIO in CMU */
	CMU_ClockEnable(cmuClock_GPIO, true);
	gpioDisableInterrupts();
}

void gpio_IntrPin_Setup(void) {
	/* Register callbacks before setting up and enabling pin interrupt. */
	GPIOINT_CallbackRegister(7, gpioCallback);

	/* Configure PB9 as input */
	GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0);

	/* Set falling edge interrupt */
	GPIO_IntConfig(gpioPortC, 7, false, true, true);
}

void gpio_buttons_Setup(void) {
	GPIOINT_CallbackRegister(9, gpioCallback);
	GPIOINT_CallbackRegister(10, gpioCallback);

	/* Configure PB9 and PB10 as input */
	GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

	/* Set falling edge interrupt for both ports */
	GPIO_IntConfig(gpioPortB, 9, false, true, true);
	GPIO_IntConfig(gpioPortB, 10, false, true, true);
}

void gpioDisableInterrupts(void) {
	NVIC_DisableIRQ(GPIO_ODD_IRQn);
	NVIC_DisableIRQ(GPIO_EVEN_IRQn);
}

void gpioEnableInterrupts(void) {
	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}
