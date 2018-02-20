/*
 * i2c_hard.h
 *
 *  Created on: Feb 16, 2018
 *      Author: Administrator
 */

#ifndef SRC_GPIO_LIB_I2C_HARD_H_
#define SRC_GPIO_LIB_I2C_HARD_H_

#define BUFFER_SIZE 	16
#define SHORT_DELAY		5
#define LONG_DELAY		10
#define SDA_PIN			8
#define SCL_PIN			9


typedef enum MODE {
	HIGH,
	LOW,
} mode;

typedef enum IN_OUT {
	OUTPUT,
	INPUT,
} in_out;

typedef enum BIT_TYPE {
	ONE,
	ZERO,
} bit_type;

//PF9 is SCL. PF8 is SDA
//Many static functions in i2c_hard.c

void i2c_buffer_setValue(char* string);
void read_data(uint8_t* buffer, uint8_t address, uint16_t data_length);
void write_data(uint8_t* buffer, uint8_t address, uint16_t data_length);
void setup_hard_i2c(void);

void write_read_data(uint8_t* buffer, uint8_t address, uint8_t word_address, uint16_t data_length);
void write_write_data(uint8_t* buffer, uint8_t address, uint8_t word_address, uint16_t data_length);

void I2C_test(void);

#endif /* SRC_GPIO_LIB_I2C_HARD_H_ */
