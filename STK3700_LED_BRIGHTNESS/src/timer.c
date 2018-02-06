/*
 * timer.c
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include "em_timer.h"
#include "em_cmu.h"
#include "timer.h"

TIMER_Init_TypeDef timer_type_local =
{ 		.enable = true,                  	// Start timer upon configuration
		.debugRun = true,   				// Keep timer running even on debug halt
		.prescale = timerPrescale1024, 		// Use /1 prescaler...timer clock = HF clock = 1 MHz
		.clkSel = timerClkSelHFPerClk, 		// Set HF peripheral clock as clock source
		.fallAction = timerInputActionNone, // No action on falling edge
		.riseAction = timerInputActionNone, // No action on rising edge
		.mode = timerModeUp,              	// Use up-count mode
		.dmaClrAct = false,                	// Not using DMA
		.quadModeX4 = false,               	// Not using quad decoder
		.oneShot = false,          			// Using continuous, not one-shot
		.sync = false, 						// Not synchronizing timer operation off of other timers
};

void setupTimers(enum enumTimer timer, uint32_t max_value) {

	CMU_ClockEnable(cmuClock_GPIO, true);

	switch (timer) {
		case 0:
			/* Enable Timer0 */
			CMU_ClockEnable(cmuClock_TIMER0, true);
			TIMER_TopSet(TIMER0, max_value);					// Timer0
			break;
		case 1:
			/* Enable Timer1 */
			CMU_ClockEnable(cmuClock_TIMER1, true);
			TIMER_TopSet(TIMER1, max_value);					// Timer1
			break;
		case 2:
			/* Enable Timer2 */
			CMU_ClockEnable(cmuClock_TIMER2, true);
			TIMER_TopSet(TIMER2, max_value);					// Timer2
			break;
		default:
			break;
	}

}

void startTimer(enum enumTimer timer) {

	switch (timer) {
		case TIMER_0:
			TIMER_Init(TIMER0, &timer_type_local);			// Start timer0
			break;
		case TIMER_1:
			TIMER_Init(TIMER1, &timer_type_local);			// Start timer1
			break;
		case TIMER_2:
			TIMER_Init(TIMER2, &timer_type_local);			// Start timer2
			break;
		default:
			break;
	}
}


void setTimerType (enum enumPrescale prescale, enum enumFrequency frequency) {

	switch (frequency) {
		case MHZ_1:
			CMU_HFRCOBandSet(cmuHFRCOBand_1MHz);		// High frequency RC oscillator set at 1MHz
			break;
		case MHZ_11:
			CMU_HFRCOBandSet(cmuHFRCOBand_11MHz);		// High frequency RC oscillator set at 11MHz
			break;
		case MHZ_28:
			CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);		// High frequency RC oscillator set at 28MHz
			break;
		default:
			break;
	}

	switch (prescale) {
		case PSCALE_1:
			timer_type_local.prescale = timerPrescale1;			//Prescale (divide) frequency by 1
			break;
		case PSCALE_128:
			timer_type_local.prescale = timerPrescale128;		//Prescale (divide) frequency by 128
			break;
		case PSCALE_1024:
			timer_type_local.prescale = timerPrescale1024;		//Prescale (divide) frequency by 1024
			break;
		default:
			break;
	}
}

void INTR_EN_timer(enum enumTimer timer) {

	switch (timer) {
		case TIMER_0:
			// Enable timer interrupt on overflow
			TIMER_IntEnable(TIMER0, TIMER_IF_OF);
			/* Enable Timer0 interrupt in core */
			NVIC_ClearPendingIRQ(TIMER0_IRQn);
			NVIC_EnableIRQ(TIMER0_IRQn);
			break;
		case TIMER_1:
			// Enable timer interrupt on overflow
			TIMER_IntEnable(TIMER1, TIMER_IF_OF);
			/* Enable Timer0 interrupt in core */
			NVIC_ClearPendingIRQ(TIMER1_IRQn);
			NVIC_EnableIRQ(TIMER1_IRQn);
			break;
		case TIMER_2:
			// Enable timer interrupt on overflow
			TIMER_IntEnable(TIMER2, TIMER_IF_OF);
			/* Enable Timer0 interrupt in core */
			NVIC_ClearPendingIRQ(TIMER2_IRQn);
			NVIC_EnableIRQ(TIMER2_IRQn);
			break;
		default:
			break;
	}
}




