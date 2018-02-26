/*
 * buttons.h
 *
 *  Created on: Feb 13, 2018
 *      Author: Administrator
 */

#ifndef SRC_BUTTONS_H_
#define SRC_BUTTONS_H_

void GPIO_EN_buttons(void);
void GPIO_EN_Intr(void);
void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);

#endif /* SRC_BUTTONS_H_ */
