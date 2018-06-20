/*
 * SystemTick.c
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */
#include "SystemTick.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/**************************************************************************//**
 * @brief SysTick
 * Setup
 *****************************************************************************/
void SystemTick_Setup(void) {
	// Setup SysTick Timer for 1 msec interrupts
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
		while (1) ;
}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void) {
	msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void DelayInMS(uint32_t dlyTicks)
{
    uint32_t curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief SoftTimer with 1ms resolution
 * @param
 *****************************************************************************/
uint32_t GetMilliseconds(void) {
	return msTicks;
}
