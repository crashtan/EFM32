/*
 * Uart.h
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"

#define uart_rx_size 128
extern uint32_t uart_rx_count;
extern uint8_t uart_rx_data[uart_rx_size];

void Setup_UART (USART_TypeDef* UART_number);
void ReadUART_8 (USART_TypeDef* UART_number);
void ReadUART_16 (USART_TypeDef* UART_number);
void WriteUART_8(USART_TypeDef* UART_number, uint8_t data);
void WriteUART_16(USART_TypeDef* UART_number, uint16_t data);
void uart_Flush_RX_Buffer (void);

void UART1_RX_IRQHandler (void);
//void UART1_TX_IRQHandler (void);

#endif /* UART_H_ */
