/*
 * uart.c
 *
 *  Created on: Feb 13, 2018
 *      Author: Administrator
 */
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "uart.h"
#include "../RS232_cmd.h"
#include "segmentlcd.h"

/*******************************************************************************************/

void GPIO_EN_UART(void) {

	CMU_ClockDivSet(cmuClock_HF, cmuClkDiv_2);       // Set HF clock divider to /2 to keep core frequency < 32MHz
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);   // Enable XTAL Osc and wait to stabilize
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO); // Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock will be 24MHz

	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);          // Enable USART1 peripheral clock

	/* Set Modes for OUTPUT and INPUT Pins respectively */
	GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);

	/* Initialize UART */
	USART_InitAsync_TypeDef uartInit =
	{
		.enable       = usartDisable,   // Wait to enable the transmitter and receiver
		.refFreq      = 0,              // Setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
		.baudrate     = 57600,          // Desired baud rate
		.oversampling = usartOVS16,     // Set oversampling value to x16
		.databits     = usartDatabits8, // 8 data bits
		.parity       = usartOddParity,  // No parity bits
		.stopbits     = usartStopbits2, // 1 stop bit
		.mvdis        = false,          // Use majority voting
		.prsRxEnable  = false,          // Not using PRS input
		.prsRxCh      = usartPrsRxCh0,  // Doesn't matter which channel we select
	};

	USART_InitAsync(USART1, &uartInit);
	USART1->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	/* Set rising edge interrupt for RX and enable interrupts */
	GPIO_IntConfig(gpioPortD, 1, false, true, true);

	USART_IntClear(USART1, _UART_IF_MASK); // Clear any USART interrupt flags
	USART1->IEN = USART_IEN_RXDATAV;

	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);
	NVIC_DisableIRQ(USART1_RX_IRQn);

	USART_Enable(USART1, usartEnable);     // Enable transmitter and receiver
}
/*******************************************************************************************/
//PD0 (UART1_RX)
void USART1_RX_IRQHandler(void) {
	GPIO_IntClear(1<<11);

	char temp = '\0';
	temp = receiveChar();

	//RECEIVE DATA
	if (USART1->RXDATA) {
		receivedString[0] = temp;
		SegmentLCD_Write(receivedString);
	}
}

/*******************************************************************************************/
