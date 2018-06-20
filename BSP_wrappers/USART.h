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

#define rx_size 512
extern uint32_t rx_count;
extern uint8_t rx_data[rx_size];

void Setup_USART (USART_TypeDef* usart_number);
void ReadUSART_8 (USART_TypeDef* usart_number);
void ReadUSART_16 (USART_TypeDef* usart_number);
void WriteUSART_8(USART_TypeDef* usart_number, uint8_t data);
void WriteUSART_16(USART_TypeDef* usart_number, uint16_t data);
void Flush_RX_Buffer (void);

void USART1_RX_IRQHandler (void);
//void USART1_TX_IRQHandler (void);

#endif /* UART_H_ */
