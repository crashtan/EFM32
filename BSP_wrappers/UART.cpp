/*
 * Uart.c
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#include "UART.h"

uint8_t uart_rx_data[uart_rx_size] = {0};
uint32_t uart_rx_count = 0;

void Setup_UART(USART_TypeDef* UART_number){

	//============
	// PARAMETERS
	//============
	uint32_t 			UART_BAUD_RATE 		= 115200;
	CMU_Clock_TypeDef 	cmuClock_UART 			;
	IRQn_Type 			UART_RX_IRQn 			;
	IRQn_Type 			UART_TX_IRQn 			;
	uint32_t 			UART_ROUTE_LOCATION 	;

	if (UART_number == UART1) {
		cmuClock_UART 		= cmuClock_UART1;
		UART_RX_IRQn 		= UART1_RX_IRQn;
		UART_TX_IRQn 		= UART1_TX_IRQn;
		UART_ROUTE_LOCATION = UART_ROUTE_LOCATION_LOC1;
		GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);	//TX
		GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);	//RX
		/* Set rising edge interrupt for RX and enable interrupts */
		GPIO_IntConfig(gpioPortD, 1, false, true, true);
	}

	//=======
	// SETUP
	//=======
	// Enable clock for UART module
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_UART, true);

	USART_InitAsync_TypeDef UARTInit = USART_INITASYNC_DEFAULT;

	// Prepare struct for initialising UART in asynchronous mode
	UARTInit.enable       = usartDisable;   // Don't enable UART upon initialisation
//	UARTInit.refFreq      = 0;              // Provide information on reference frequency. When set to 0, the reference frequency is
	UARTInit.baudrate     = UART_BAUD_RATE; // Baud rate
//	UARTInit.oversampling = usartOVS16;     // Over-sampling. Range is 4x, 6x, 8x or 16x
//	UARTInit.databits     = usartDatabits8; // Number of data bits. Range is 4 to 10
	UARTInit.parity       = usartNoParity;  //Odd Parity mode
	UARTInit.stopbits     = usartStopbits1; // Number of stop bits. Range is 0 to 2
//	UARTInit.mvdis        = false;          // Disable majority voting
//	UARTInit.prsRxEnable  = false;          // Enable UART Rx via Peripheral Reflex System
//	UARTInit.prsRxCh      = usartPrsRxCh0;  // Select PRS channel if enabled

	// Initialize UART with UARTInit struct
	USART_InitAsync(UART_number, &UARTInit);

	// Prepare UART RX and TX interrupts
	USART_IntClear(UART_number, _UART_IF_MASK);
	USART_IntEnable(UART_number, UART_IF_RXDATAV);

	NVIC_ClearPendingIRQ(UART_RX_IRQn);
	NVIC_ClearPendingIRQ(UART_TX_IRQn);

	NVIC_EnableIRQ(UART_RX_IRQn);
	NVIC_EnableIRQ(UART_TX_IRQn);

	// Enable I/O pins at UART location
	UART_number->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION;
	//UART_number->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	// Enable UART
	USART_Enable(UART_number, usartEnable);
}
//======================================================================================

void ReadUART_8 (USART_TypeDef* UART_number) {
	rx_count = 0;
	if (UART_number->RXDATA) {
		if (uart_rx_count == uart_rx_size){
			uart_rx_count = 0;
		}
		uart_rx_data[uart_rx_count++] = UART_number->RXDATA;
	}
}

void ReadUART_16 (USART_TypeDef* UART_number) {
	if (UART_number->RXDOUBLE){
		//TODO:
	}
}

void WriteUART_8(USART_TypeDef* UART_number, uint8_t data) {
	while (!(UART_number->STATUS & UART_STATUS_TXBL));
	UART_number->TXDATA = (uint32_t)data;
}

void WriteUART_16(USART_TypeDef* UART_number, uint16_t data) {
	while (!(UART_number->STATUS & UART_STATUS_TXBL));
	UART_number->TXDOUBLE = (uint32_t)data;
}

extern "C" void UART1_RX_IRQHandler (void) {
	Flush_RX_Buffer();

	while(UART1->STATUS & (1 << 7)) {
		ReadUART_8(UART1);
	}

	USART_IntClear(UART1, UART_IF_RXDATAV);
}

void uart_Flush_RX_Buffer (void) {
	for (uint32_t i=0; i < uart_rx_size; i++) {
		uart_rx_data[i] = 0;
	}
}

//void UART1_TX_IRQHandler (void) {
//	//TODO: Not needed atm
//}
