/*
 * I2C.h
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"

#define I2C_DEFAULT_FREQ 1000000; 	//1MHz
//#define I2C_DEFAULT_FREQ 400000;	//400kHz
//#define I2C_DEFAULT_FREQ 100000;	//100kHz
//#define I2C_DEFAULT_FREQ 85000;	//85kHz
//#define I2C_DEFAULT_FREQ 79000;	//79kHz
//#define I2C_DEFAULT_FREQ 52000;	//52kHz

void I2C1_Setup(void);
void PerformI2C_READ(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *ReadBuffer, uint16_t Length);
void PerformI2C_WRITE(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *WriteBuffer, uint16_t Length);

#endif /* I2C_H_ */
