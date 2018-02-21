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
#define LONG_DELAY		5
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
//Static functions in i2c_hard.c

void setup_hard_i2c(void);

void read_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint16_t uint16_t_data_length);
void write_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint16_t uint16_t_data_length);
void write_read_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint8_t uint8_t_word_address, uint16_t uint16_t_data_length);
void write_write_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint8_t uint8_t_word_address, uint16_t uint16_t_data_length);

void I2C_hard_test(uint8_t uint8_t_test_address, uint8_t* uint8_t_input_buffer, uint8_t* uint8_t_output_buffer);

#endif /* SRC_GPIO_LIB_I2C_HARD_H_ */
