/*
 * i2c.h
 *
 *  Created on: Feb 13, 2018
 *      Author: Administrator
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "em_i2c.h"
#include "stdint.h"


#define I2C_ADDRESS_DS1307				0x68	//7 bit address, need to shift left 1 bit
#define EEPROM_ADDRESS					0x50	//7 bit address, need to shift left 1 bit
#define I2C_BUFFER_SIZE					8

extern uint8_t i2c_rxBuffer[I2C_BUFFER_SIZE];
extern uint8_t i2c_txBuffer[I2C_BUFFER_SIZE];

void setup_I2C(void);
void I2C1_IRQHandler(void);

void enableI2cSlaveInterrupts(void);
void disableI2cInterrupts(void);
void i2c_writeData(uint8_t address);
void i2c_readData(uint8_t address);

#endif /* SRC_I2C_H_ */


