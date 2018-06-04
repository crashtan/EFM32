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

#define I2C_DEFAULT_FREQ 100000; //100kHz or 400kHz

void SetupI2C1(void);
void PerformI2CRead(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *ReadBuffer, uint16_t Length);
void PerformI2CWrite(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *WriteBuffer, uint16_t Length);

#endif /* I2C_H_ */
