#include "em_chip.h"
#include "timer.h"
#include "led.h"
#include "GPIO_led.h"
#include "PWM.h"


int main(void)
{
	/* Chip errata */
	CHIP_Init();

	/* Initialize GPIO */
	setupGPIO();
	GPIO_EN_buttons();

	/* Initialize LEDS */
	setupLED();
	turnOnLeds();

	/* Initialize TIMER */
	setTimerType (PSCALE_1, MHZ_11);
	setupTimers(TIMER_0, 1000);
	INTR_EN_timer(TIMER_0);
	startTimer(TIMER_0);

	/* Infinite loop */
	while (1) {
		runLedPWM();
  }
}
