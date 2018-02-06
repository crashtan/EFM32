/*
 * GPIO_led.h
 *
 *  Created on: Feb 2, 2018
 *      Author: Administrator
 */
#include <stdint.h>

#ifndef SRC_GPIO_LED_H_
#define SRC_GPIO_LED_H_

void setupGPIO(void);
void GPIO_EN_buttons(void);
void TIMER0_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);

#endif /* SRC_GPIO_LED_H_ */
