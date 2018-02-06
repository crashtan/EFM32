/***************************************************************************//**
 * @file
 * @brief Simple GPIO interrupt dispatcher Demo for EFM32GG_STK3700
 * @version 5.3.5
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "segmentlcd.h"
#include "em_rtc.h"
#include "em_timer.h"
#include "vddcheck.h"
#include "lightsense_conf.h"
#include "em_lesense.h"


#define MODE_CLOCK_GPIO 		0
#define MODE_TIMER_GPIO 		1
#define MODE_LED_GPIO 			2
#define MODE_BRIGHTNESS_GPIO	3
volatile uint8_t programMode = MODE_BRIGHTNESS_GPIO;

//RTC ***************************************************************************/

#define RTC_FREQ    32768
uint16_t clock_hours 	= 0;
uint16_t clock_minutes 	= 0;

//TIMER0 ***************************************************************************/

uint16_t timer_seconds = 0;
uint16_t timer_minutes = 0;
uint8_t timer_flag 	= 0;

TIMER_Init_TypeDef timer_type0 =
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

//TIMER1 ***************************************************************************/

volatile uint16_t msCounter = 0;
uint16_t PWM_value = 101;
uint16_t threshold_factor = 0;

TIMER_Init_TypeDef timer_type1 =
{ 		.enable = true,                  	// Start timer upon configuration
		.debugRun = true,   				// Keep timer running even on debug halt
		.prescale = timerPrescale1, 		// Use /1 prescaler...timer clock = HF clock = 1 MHz
		.clkSel = timerClkSelHFPerClk, 		// Set HF peripheral clock as clock source
		.fallAction = timerInputActionNone, // No action on falling edge
		.riseAction = timerInputActionNone, // No action on rising edge
		.mode = timerModeUp,              	// Use up-count mode
		.dmaClrAct = false,                	// Not using DMA
		.quadModeX4 = false,               	// Not using quad decoder
		.oneShot = false,          			// Using continuous, not one-shot
		.sync = false, 						// Not synchronizing timer operation off of other timers
};


//FUNCTION PROTOTYPES ********************************************************/

void LESENSE_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER0_IRQHandler(void);
void RTC_IRQHandler(void);
void setupLESENSE(void);
void setupRTC(void);
void setupTimer0(void);
void setupTimer1(void);
void setupGPIO(void);
void setupLED(void);

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/

int main(void) {

	/* Chip errata */
	CHIP_Init();

	/* Initialize gpio */
	setupGPIO();

	/* Initialize LED setup */
	setupLED();

	/*Initialize LCD with Boost On/Off */
	SegmentLCD_Init(false);

	/*Initialize RTC */
	setupRTC();

	/* Infinite loop */
	START:

	GPIO_PinOutClear(gpioPortE, 2);
	GPIO_PinOutClear(gpioPortE, 3);

	while (1) {
		switch (programMode) {

			case MODE_CLOCK_GPIO:
				//CLOCK MODE
				SegmentLCD_Write("Clock");
				SegmentLCD_Number(clock_hours*100 + clock_minutes);
				SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
				break;

			case MODE_TIMER_GPIO:
				//TIMER MODE
				setupTimer0(); 	// 1s interrupt

				SegmentLCD_Write("Timer");
				SegmentLCD_Number(timer_minutes*100 + timer_seconds);
				SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
				break;

			case MODE_LED_GPIO:
				//LED MODE
				SegmentLCD_Write("LED");
				  break;

			case MODE_BRIGHTNESS_GPIO:
				//RIGHT LED PWM MODE
				SegmentLCD_Write("ledPWM");

				GPIO_PinOutSet(gpioPortE, 2);
				GPIO_PinOutSet(gpioPortE, 3);

				setupTimer1();	// 0.01s interrupt for PWM
				TIMER_Init(TIMER1, &timer_type1);

				while (1) {
					//Change brightness of RIGHT LED
					while (msCounter <= PWM_value-threshold_factor) {}
					msCounter = 0;
					GPIO_PinOutClear(gpioPortE, 2);
					while (msCounter <= PWM_value+threshold_factor) {}
					msCounter = 0;
					GPIO_PinOutSet(gpioPortE, 2);
				}
				break;

			default:
				break;
		}
	}
}


/***************************************************************************//**
 * @brief  Light Sensor callback
 * @param
 ******************************************************************************/

void LESENSE_IRQHandler(void) {
	//Clear Interrupt
	/* Negative edge interrupt on LESENSE CH6. */
	if (LESENSE_IF_CH6 & LESENSE_IntGetEnabled()) {
	LESENSE_IntClear(LESENSE_IF_CH6);
	}
	//Change programMode
	programMode = (programMode+1)%4;
	goto START;
}

/***************************************************************************//**
 * @brief  GPIO button callbacks
 * @param
 ******************************************************************************/

//PB0 (PB9)  ******************************************************************/
void GPIO_ODD_IRQHandler(void) {
	//Clear ODD Interrupt
	GPIO_IntClear(1<<9);

	switch (programMode) {

	case MODE_CLOCK_GPIO:
		clock_hours = (clock_hours+1) % 24;
		break;

	case MODE_TIMER_GPIO:
		timer_flag = (timer_flag+1)%2;
		if (timer_flag == 1)
			TIMER_Init(TIMER0, &timer_type0);			// Start timer
		else
			TIMER0->CMD = TIMER_CMD_STOP;				// Stop Timer
		break;

	case MODE_LED_GPIO:
		GPIO_PinOutToggle(gpioPortE, 3);
		break;

	case MODE_BRIGHTNESS_GPIO:
		/* DECREASE BRIGHTNESS */
		if ( threshold_factor <= 100 )
			threshold_factor += 10;
		break;

	default:
		break;
	}
}

//PB1 (PB10)  ******************************************************************/
void GPIO_EVEN_IRQHandler(void) {
	//Clear EVEN Interrupt
	GPIO_IntClear(1<<10);

	switch (programMode) {

	case MODE_CLOCK_GPIO:
		clock_minutes = (clock_minutes+1) % 60;
		break;

	case MODE_TIMER_GPIO:
		//Reset Timer
		timer_seconds = 0;
		timer_minutes = 0;
		break;

	case MODE_LED_GPIO:
		GPIO_PinOutToggle(gpioPortE, 2);
		break;

	case MODE_BRIGHTNESS_GPIO:
		/* INCREASE BRIGHTNESS */
		if ( threshold_factor >= 10 )
			threshold_factor -=10;
		break;

	default:
		break;
	}
}

/***************************************************************************//**
 * // @brief: TIMER GPIO interrupt Handler functions
 *
 ******************************************************************************/
void TIMER1_IRQHandler(void) {
	//Clear Interrupt Request
	TIMER_IntClear(TIMER1, TIMER_IF_OF);
	msCounter++;
}

void TIMER0_IRQHandler(void) {
	TIMER_IntClear(TIMER0, TIMER_IF_OF);        // Clear overflow flag
	timer_seconds++;                            // Increment counter

	if (timer_seconds > 59) {
		timer_seconds = 0;
		timer_minutes++;
	}
	if (timer_minutes > 99) {
		timer_minutes = 0;
	}
}

/***************************************************************************//**
 * @brief RTC Interrupt Handler.
 *        Updates minutes and hours.
 ******************************************************************************/
void RTC_IRQHandler(void)
{
  /* Clear interrupt source */
  RTC_IntClear(RTC_IFC_COMP0);

  /* Increase time by one minute */
  clock_minutes++;
  if (clock_minutes > 59) {
    clock_minutes = 0;
    clock_hours++;
    if (clock_hours > 23) {
      clock_hours = 0;
    }
  }
}

/***************************************************************************//**
 * @brief  Setup the LESENSE
 ******************************************************************************/

void setupLESENSE(void) {

	/* Enable clock for LESENSE. */
	CMU_ClockEnable(cmuClock_LESENSE, true);
	/* Enable clock divider for LESENSE. */
	CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);
	/* LESENSE channel configuration constant table. */
	static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSE_SCAN_CONF;
	/* LESENSE alternate excitation channel configuration constant table. */
	static const LESENSE_ConfAltEx_TypeDef initAltEx = LESENSE_LIGHTSENSE_ALTEX_CONF;
	/* LESENSE central configuration constant table. */
	static const LESENSE_Init_TypeDef initLESENSE =
	{
	.coreCtrl =
	{
	  .scanStart = lesenseScanStartPeriodic,
	  .prsSel = lesensePRSCh0,
	  .scanConfSel = lesenseScanConfDirMap,
	  .invACMP0 = false,
	  .invACMP1 = false,
	  .dualSample = false,
	  .storeScanRes = false,
	  .bufOverWr = true,
	  .bufTrigLevel = lesenseBufTrigHalf,
	  .wakeupOnDMA = lesenseDMAWakeUpDisable,
	  .biasMode = lesenseBiasModeDutyCycle,
	  .debugRun = false
	},

	.timeCtrl =
	{
	  .startDelay = 0U
	},

	.perCtrl =
	{
	  .dacCh0Data = lesenseDACIfData,
	  .dacCh0ConvMode = lesenseDACConvModeDisable,
	  .dacCh0OutMode = lesenseDACOutModeDisable,
	  .dacCh1Data = lesenseDACIfData,
	  .dacCh1ConvMode = lesenseDACConvModeDisable,
	  .dacCh1OutMode = lesenseDACOutModeDisable,
	  .dacPresc = 0U,
	  .dacRef = lesenseDACRefBandGap,
	  .acmp0Mode = lesenseACMPModeMuxThres,
	  .acmp1Mode = lesenseACMPModeMuxThres,
	  .warmupMode = lesenseWarmupModeNormal
	},

	.decCtrl =
	{
	  .decInput = lesenseDecInputSensorSt,
	  .initState = 0U,
	  .chkState = false,
	  .intMap = true,
	  .hystPRS0 = false,
	  .hystPRS1 = false,
	  .hystPRS2 = false,
	  .hystIRQ = false,
	  .prsCount = true,
	  .prsChSel0 = lesensePRSCh0,
	  .prsChSel1 = lesensePRSCh1,
	  .prsChSel2 = lesensePRSCh2,
	  .prsChSel3 = lesensePRSCh3
	}
	};

	/* Initialize LESENSE interface with RESET. */
	LESENSE_Init(&initLESENSE, true);

	/* Configure scan channels. */
	LESENSE_ChannelAllConfig(&initChs);

	/* Configure alternate excitation channels. */
	LESENSE_AltExConfig(&initAltEx);

	/* Set scan frequency (in Hz). */
	(void)LESENSE_ScanFreqSet(0U, 20U);

	/* Set clock divisor for LF clock. */
	LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

	/* Start scanning LESENSE channels. */
	LESENSE_ScanStart();
}

/***************************************************************************//**
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every minute.
 ******************************************************************************/

void setupRTC(void)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  /* Enable LE domain registers */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable LFXO as LFACLK in CMU. This will also start LFXO */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Set a clock divisor of 32 to reduce power conumption. */
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32);

  /* Enable RTC clock */
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Initialize RTC */
  rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtcInit.debugRun = false;  /* Halt RTC when debugging. */
  rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */
  RTC_Init(&rtcInit);

  /* Interrupt every minute */
  RTC_CompareSet(0, ((RTC_FREQ / 32) * 60) - 1);

  /* Enable interrupt */
  RTC_IntEnable(RTC_IEN_COMP0);
  NVIC_EnableIRQ(RTC_IRQn);

  /* Start Counter */
  RTC_Enable(true);
}

/***************************************************************************//**
 * @brief Enables HFCLK and selects HFRCO as clock source for TIMER0
 *        Sets up the TIMER0.
 ******************************************************************************/

void setupTimer0(void) {

	CMU_HFRCOBandSet(cmuHFRCOBand_1MHz);		// High frequency RC oscillator set at 1MHz

	/* Enable Timer0 */
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_TIMER0, true);

	TIMER_TopSet(TIMER0, 1000);					// 1
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);		// Enable timer interrupt on overflow

	/* Enable Timer0 interrupt in core */
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);

	//Don't start timer yet. Only on GPIO interrupt
}

/***************************************************************************//**
 * @brief Enables HFCLK and selects HFRCO as clock source for TIMER1
 *        Sets up the TIMER1.
 ******************************************************************************/

void setupTimer1(void) {

	CMU_HFRCOBandSet(cmuHFRCOBand_11MHz);		// High frequency RC oscillator set at 1MHz

	/* Enable Timer0 */
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_TIMER1, true);

	TIMER_TopSet(TIMER1, 100);					// 1
	TIMER_IntEnable(TIMER1, TIMER_IF_OF);		// Enable timer interrupt on overflow

	/* Enable Timer0 interrupt in core */
	NVIC_ClearPendingIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);

	//Don't start timer yet. Only on GPIO interrupt
}

/***************************************************************************//**
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.

 ******************************************************************************/
void setupGPIO(void) {

	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Configure PB9 and PB10 as input */
	GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

	/* Set falling edge interrupt for both ports */
	GPIO_IntConfig(gpioPortB, 9, false, true, true);
	GPIO_IntConfig(gpioPortB, 10, false, true, true);

	/* Enable interrupt in core for even and odd gpio interrupts */
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief  LED INITIALIZER
 *
 ******************************************************************************/
void setupLED(void) {
	/* Enable GPIO in CMU */
	CMU_ClockEnable(cmuClock_GPIO, true);					//Enable GPIO clock

	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);	//Set LED0 as digital output
	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);	//Set LED1 as digital output
}

