/*
 * Uart.c
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#include "USART.h"

uint8_t rx_data = 0;

void SetupUSART(USART_TypeDef* usart_number){

	//============
	// PARAMETERS
	//============
	uint32_t 			USART_BAUD_RATE 		= 115200;
	CMU_Clock_TypeDef 	cmuClock_USART 			;
	IRQn_Type 			USART_RX_IRQn 			;
	IRQn_Type 			USART_TX_IRQn 			;
	uint32_t 			USART_ROUTE_LOCATION 	;

	if (usart_number == USART1) {
		cmuClock_USART 		= cmuClock_USART1;
		USART_RX_IRQn 		= USART1_RX_IRQn;
		USART_TX_IRQn 		= USART1_TX_IRQn;
		USART_ROUTE_LOCATION = USART_ROUTE_LOCATION_LOC1;
		GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);	//TX
		GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);	//RX
		/* Set rising edge interrupt for RX and enable interrupts */
		GPIO_IntConfig(gpioPortD, 1, false, true, true);
	}

	//=======
	// SETUP
	//=======
	// Enable clock for USART module
	CMU_ClockEnable(cmuClock_USART, true);

	USART_InitAsync_TypeDef usartInit = USART_INITASYNC_DEFAULT;

	// Prepare struct for initialising USART in asynchronous mode
	usartInit.enable       = usartDisable;   // Don't enable UART upon initialisation
//	usartInit.refFreq      = 0;              // Provide information on reference frequency. When set to 0, the reference frequency is
	usartInit.baudrate     = USART_BAUD_RATE; // Baud rate
//	usartInit.oversampling = usartOVS16;     // Over-sampling. Range is 4x, 6x, 8x or 16x
//	usartInit.databits     = usartDatabits8; // Number of data bits. Range is 4 to 10
	usartInit.parity       = usartOddParity;  //Odd Parity mode
	usartInit.stopbits     = usartStopbits2; // Number of stop bits. Range is 0 to 2
//	usartInit.mvdis        = false;          // Disable majority voting
//	usartInit.prsRxEnable  = false;          // Enable USART Rx via Peripheral Reflex System
//	usartInit.prsRxCh      = usartPrsRxCh0;  // Select PRS channel if enabled

	// Initialize USART with usartInit struct
	USART_InitAsync(usart_number, &usartInit);

	// Prepare USART RX and TX interrupts
	USART_IntClear(usart_number, _USART_IF_MASK);
	USART_IntEnable(usart_number, USART_IF_RXDATAV);

	NVIC_ClearPendingIRQ(USART_RX_IRQn);
	NVIC_ClearPendingIRQ(USART_TX_IRQn);

	NVIC_EnableIRQ(USART_RX_IRQn);
	NVIC_EnableIRQ(USART_TX_IRQn);

	// Enable I/O pins at USART location
	usart_number->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION;
	//usart_number->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	// Enable USART
	USART_Enable(usart_number, usartEnable);
}
//======================================================================================

uint8_t ReadUSART_8 (USART_TypeDef* usart_number) {
	volatile uint8_t data = 0;
	if (usart_number->RXDATA)
			data = usart_number->RXDATA;
	return data;
}

uint16_t ReadUSART_16 (USART_TypeDef* usart_number) {
	volatile uint8_t data = 0;
	if (usart_number->RXDOUBLE)
		data = usart_number->RXDOUBLE;
	return data;
}

void WriteUSART_8(USART_TypeDef* usart_number, uint8_t data) {
	while (!(usart_number->STATUS & USART_STATUS_TXBL));
	usart_number->TXDATA = (uint32_t)data;
}

void WriteUSART_16(USART_TypeDef* usart_number, uint16_t data) {
	while (!(usart_number->STATUS & USART_STATUS_TXBL));
	usart_number->TXDOUBLE = (uint32_t)data;
}

void USART1_RX_IRQHandler (void) {
	GPIO_IntClear(1<<11);

	rx_data = ReadUSART_8(USART1);
	//ReadUSART_16(USART1);
}

//void USART1_TX_IRQHandler (void) {
//	//TODO: Not needed atm
//}
