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
#include "GPIO_lib/i2c_hard.h"

//PROGRAM
#include "RS232_cmd.h"

/***************************************************************************//**
 * @brief GLOBAL VARIABLES
 ******************************************************************************/
#define DS1307_ADDRESS 	0x68
#define ATMEL_EEPROM_24C04N_ADDRESS 	0x50

uint8_t clock_hours;
uint8_t clock_minutes;

uint8_t test_buffer[64];


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

int main(void)
{
	// Chip errata
	CHIP_Init();

	//Set Clock Type and Enable CPU Clock
	cmuSetup();

	// Enable USART
	GPIO_EN_UART();

	/*
	// Initialize 1ms TIMER0
	setTimerType (PSCALE_1, MHZ_11);		//Timer frequency and prescaling
	setupTimers(TIMER_0, 10000);		//Timer top time
	INTR_EN_timer(TIMER_0);				//Enable Timer interrupt on overflow
	startTimer(TIMER_0);				//start timer
	//Set Timer Counter 0
	milliCounter = 0;
	*/

	// Initialize LCD with Boost On/Off
	SegmentLCD_Init(false);

	// Start LEDS
	setupLED();

	setup_hard_i2c();

	//Setup I2C soft
	setup_I2C();

	//delay to stabilize i2c peripheral
	delayInMS(50000);

	/***************************************************************************//**
	 * @brief PROGRAM START
	 ******************************************************************************/

	//First string on opening program
	sendString("\n\r Hello!");
	sendString(" Welcome to RS232 on STK3700.\n\r");

	/* Infinite loop */
	while (1) {

		NVIC_DisableIRQ(USART1_RX_IRQn);

		static char count = 'A';
		char* test_input = &count;
		I2C_hard_test(ATMEL_EEPROM_24C04N_ADDRESS, test_input, test_buffer);
		count++;

		NVIC_ClearPendingIRQ(USART1_RX_IRQn);
		NVIC_EnableIRQ(USART1_RX_IRQn);

		sendString( (char*)test_buffer );


		/*
		// I2C_SOFT_TEST
		i2c_writeData(0x00);
		i2c_readData(0x00);
		for (uint8_t i=0; i<8; i++) {
			i2c_string[i] = (char)i2c_rxBuffer[i];
		}
		sendString(i2c_string);
		*/

		/*
		// DS1307 RTC TEST
		SegmentLCD_Write("Clock");
		SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
		SegmentLCD_Number(clock_hours*100 + clock_minutes);
		*/
  }
}
