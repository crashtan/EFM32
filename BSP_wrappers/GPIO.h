/*
 * GPIO.h
 *
 *  Created on: Jun 7, 2018
 *      Author: moiz.hussain
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include "gpiointerrupt.h"
#include "em_gpio.h"
#include "em_int.h"

void gpioCallback(uint8_t pin);
void gpio_Setup(void);
void gpio_IntrPin_Setup(void);
void gpio_buttons_Setup(void);
void gpioDisableInterrupts(void);
void gpioEnableInterrupts(void);

#endif /* GPIO_H_ */
