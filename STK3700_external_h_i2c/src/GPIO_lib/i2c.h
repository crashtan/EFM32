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

#define I2C_WORD_ADDRESS_LENGTH			1

void setup_I2C(void);
void I2C1_IRQHandler(void);

void enableI2cSlaveInterrupts(void);
void disableI2cInterrupts(void);
void i2c_writeData(uint8_t address, uint8_t word_address, uint8_t* write_buffer, uint8_t write_length);
void i2c_readData(uint8_t address, uint8_t word_address, uint8_t* read_buffer, uint8_t read_length);

#endif /* SRC_I2C_H_ */


