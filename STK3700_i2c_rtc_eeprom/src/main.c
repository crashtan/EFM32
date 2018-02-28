//External
#include <string.h>
#include <stdlib.h>

//EM_LIB
#include "em_chip.h"
#include "em_cmu.h"
#include "vddcheck.h"
#include "segmentlcd.h"

//GPIO_LIB
#include "GPIO_lib/timer.h"
#include "GPIO_lib/led.h"
#include "GPIO_lib/uart.h"
#include "GPIO_lib/i2c.h"
#include "GPIO_lib/i2c_soft.h"

//PROGRAM
#include "DS1307.h"
#include "RS232_cmd.h"

/***************************************************************************//**
 * @brief GLOBAL VARIABLES
 ******************************************************************************/
typedef enum STATE {
	SET_MODE,
	RUN_MODE,
} state;

/***************************************************************************//**
 * @brief Set up Clock Management Unit
 ******************************************************************************/

void cmuSetup(void)
{
  /* Start HFXO and wait until it is stable */
  /* CMU_OscillatorEnable( cmuOsc_HFXO, true, true); */

  /* Select HFXO as clock source for HFCLK */
  /* CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO ); */

  /* Disable HFRCO */
  /* CMU_OscillatorEnable( cmuOsc_HFRCO, false, false ); */

  /* Enable clock for HF peripherals */
  CMU_ClockEnable(cmuClock_HFPER, true);
}

/***************************************************************************//**
 * @brief Initialize CHIP and PROGRAM
 ******************************************************************************/

int main(void) {
	// Chip errata
	CHIP_Init();

	//Set Clock Type and Enable CPU Clock
	cmuSetup();

	// Enable USART
	GPIO_EN_UART();

	/*
	// Initialize 1ms TIMER1
	setTimerType (PSCALE_1024, MHZ_1);		//Timer frequency and prescaling
	setupTimers(TIMER_1, 1000);			//Timer top time
	INTR_EN_timer(TIMER_1);					//Enable Timer interrupt on overflow
	startTimer(TIMER_1);
	*/

	// Initialize LCD with Boost On/Off
	SegmentLCD_Init(false);

	// Start LEDS
	//setupLED();

	// I2C setup on PF8 and PF9 GPIO
	setup_soft_i2c();

	// Setup I2C hardware
	setup_I2C();

	// Delay to stabilize i2c peripheral
	delayInMS(10000);

	/***************************************************************************//**
	 * @brief PROGRAM START
	 ******************************************************************************/

	//First string on opening program
	sendString("\n\r\n\rHello!");
	sendString(" Welcome to RS232 on STK3700.\n\r");

	// DS1307 Initialization
	sendString(" Welcome to RTC Clock on I2C\n\r");
	state prog_state = RUN_MODE;
	//update_DS1307_from_EEPROM();

	/* Infinite loop */
	while (1) {

		switch (prog_state) {
		case RUN_MODE:
			// GET DATA FROM DS1307
			update_DS1307();

			// DS1307 RTC DISPLAY
			SegmentLCD_Write(DS1307_getDate());
			SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
			SegmentLCD_Number(DS1307_getHours()*100 + DS1307_getMinutes());

			if (currentString[0] == 's')
				prog_state = SET_MODE;
			if (currentString[0] == 'x') {
				sendString("\n\rExiting Program");
				exit(0);
			}
			break;

		case SET_MODE:
			sendString("\n\rEntered Clock Set Mode");
			DS1307_setMode();
			prog_state = RUN_MODE;
			break;

		default:
			break;
		}
	}
	return 0;
}
