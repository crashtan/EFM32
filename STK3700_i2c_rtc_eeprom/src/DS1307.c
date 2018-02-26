/*
 * DS1307.c
 *
 *  Created on: Feb 23, 2018
 *      Author: Administrator
 */
//EXTERNAL
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//GPIO_LIB
#include "GPIO_lib/i2c.h"
#include "GPIO_lib/i2c_soft.h"

//PROGRAM
#include "DS1307.h"
#include "RS232_cmd.h"


uint8_t read_buffer[DS1307_BUFFER_SIZE];
uint8_t write_buffer[DS1307_BUFFER_SIZE];

uint16_t clock_hours = 0;
uint16_t clock_minutes = 0;

/*******************************************************************************************/

void update_DS1307_from_EEPROM (void) {
	//Disable Interrupts
	NVIC_DisableIRQ(USART1_RX_IRQn);

	// READ FROM EEPROM, SOFT I2C
	write_read_data(write_buffer, ATMEL_EEPROM_24C04N_ADDRESS, 0x00, DS1307_BUFFER_SIZE);

	// WRITE TO DS1307, HARDWARE I2C
	i2c_writeData(DS1307_ADDRESS, SECONDS_REGISTER, write_buffer, DS1307_BUFFER_SIZE);

	// Enable interrupts
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);

}


/*******************************************************************************************/

void update_DS1307(void) {

	bool flag = false;

	//Disable Interrupts
	NVIC_DisableIRQ(USART1_RX_IRQn);

	// READ FROM DS1307, HARDWARE I2C
	i2c_readData(DS1307_ADDRESS, SECONDS_REGISTER, read_buffer, DS1307_BUFFER_SIZE);

	// WRITE TO ATMEL EEPROM, SOFT I2C
	write_write_data(read_buffer, ATMEL_EEPROM_24C04N_ADDRESS, 0x00, DS1307_BUFFER_SIZE);

	// Copy into write buffer, so changes can be made to copy
	for (uint32_t i=0; i<DS1307_BUFFER_SIZE; i++) {
		if (write_buffer[i] != read_buffer[i]) {
			write_buffer[i] = read_buffer[i];
			flag = true;
		}
	}

	// Enable interrupts
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);

	// Print if there are any changes
	if (flag)
		print_DS1307();
}

/*******************************************************************************************/

void write_to_DS1307(void) {
	NVIC_DisableIRQ(USART1_RX_IRQn);
	// READ FROM DS1307, HARDWARE I2C
	i2c_writeData(DS1307_ADDRESS, SECONDS_REGISTER, write_buffer, DS1307_BUFFER_SIZE);
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);
}

/*******************************************************************************************/

void DS1307_set_day(weekday day_name){
		write_buffer[DAYS_REGISTER] = (uint8_t)day_name;
		write_to_DS1307();
}


void DS1307_set_date(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Date Input, Enter 2 digits Only!");
	} else {
		if (!((string[0] >='0') && (string[0] <='3')) && ((string[1] >='0') && (string[0] <='9')) ) {
			sendString("\n\rEntry out of range.");
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
			uint8_t lower =	((uint8_t)(string[1]-0x30));
			write_buffer[DATE_REGISTER] = lower|upper;
		}
	}
}


void DS1307_set_month(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Months Input, Enter 2 digits Only!");
	} else {
		if (!((string[0] >='0') && (string[0] <='1')) && ((string[1] >='0') && (string[0] <='9')) ) {
			sendString("\n\rEntry out of range.");
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
			uint8_t lower =	((uint8_t)(string[1]-0x30));
			write_buffer[MONTH_REGISTER] = lower|upper;
			write_to_DS1307();
		}
	}
}


void DS1307_set_year(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Year Input, Enter 2 digits Only!");
	} else {
		if (!((string[0] >='0') && (string[0] <='9')) && ((string[1] >='0') && (string[0] <='9')) ) {
			sendString("\n\rEntry out of range.");
		} else {
			//SET THE YEAR
			uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
			uint8_t lower =	((uint8_t)(string[1]-0x30));
			write_buffer[YEAR_REGISTER] = lower|upper;
			write_to_DS1307();
		}
	}
}

/*******************************************************************************************/

void DS1307_set_hour(char* string, day_night am_pm, clock_type _24_hr){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Hours Input, Enter 2 digits Only!");
	} else {
		if (_24_hr == _24_HOUR) {
			if (!((string[0] >='0') && (string[0] <='2')) && ((string[1] >='0') && (string[0] <='9')) ) {
				sendString("\n\rEntry out of range.");
			} else {
				if ((string[0] == '2') && (string[1] == '4')) {
					string[0] = 0;
					string[1] = 0;
				}
				//SET THE HOUR
				uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
				uint8_t lower =	((uint8_t)(string[1]-0x30));
				write_buffer[HOURS_REGISTER] = lower|upper;
				write_to_DS1307();
			}
		} else {
			// NOT 24 HOUR
			if (!((string[0] >='0') && (string[0] <='1')) && ((string[1] >='0') && (string[0] <='9')) ) {
				sendString("\n\rEntry out of range.");
			} else {
				//SET THE HOUR AND AM PM
				uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
				uint8_t lower =	((uint8_t)(string[1]-0x30));
				uint8_t bit_sum = lower|upper|0b01000000;
				if (am_pm == PM)
					bit_sum |= 0b00100000;
				write_buffer[HOURS_REGISTER] = bit_sum;
				write_to_DS1307();
			}
		}
	}
}

void DS1307_set_minute(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Minutes Input, Enter 2 digits Only!");
	} else {
		if (!((string[0] >='0') && (string[0] <='5')) && ((string[1] >='0') && (string[0] <='9')) ) {
			sendString("\n\rEntry out of range.");
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
			uint8_t lower =	((uint8_t)(string[1]-0x30));
			write_buffer[MINUTES_REGISTER] = lower|upper;
			write_to_DS1307();
		}
	}
}

void DS1307_set_second(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Seconds Input, Enter 2 digits Only!");
	} else {
		if (!((string[0] >='0') && (string[0] <='5')) && ((string[1] >='0') && (string[0] <='9')) ) {
			sendString("\n\rEntry out of range.");
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(string[0]-0x30))<<4);
			uint8_t lower =	((uint8_t)(string[1]-0x30));
			write_buffer[SECONDS_REGISTER] = lower|upper;
			write_to_DS1307();
		}
	}
}

/*******************************************************************************************/

void enable_DS1307_clock(bool enable) {
	if (enable)
		write_buffer[0] = 0b00000000;
	else
		write_buffer[0] = 0b10000000;
	write_to_DS1307();
}

/*******************************************************************************************/

void print_DS1307(void) {

	clock_hours = 0;
	clock_minutes = 0;
	char temp[2] = {'\0','\0'};

	if (ENABLE_BIT(read_buffer[SECONDS_REGISTER])){
		sendString("\n\rClock is disabled!");
	}
	else {

		//TIME
		sendString("\n\rTime is: ");
		// Hours

		// upper hour nibble
		bool flag_24 = false;
		bool flag_AM_PM = false;
		uint8_t _24hr_check = HOURS_HIGH(read_buffer[HOURS_REGISTER]);

		if (_24hr_check & 0b100) {
			// 12 HOUR ENABLED
			flag_24 = true;
				if ((_24hr_check & 0b010) > 0)
					// ITS PM TIME
					flag_AM_PM = true;
				else {
					// ITS AM TIME
				}
				temp[0] = 0x30+(char)(_24hr_check & 0b001);
				clock_hours = (uint16_t)(10*(temp[0] - 0x30));
		} else {
			// 24 HOUR ENABLED
			_24hr_check >>= 1;
			temp[0] = 0x30+(char)_24hr_check;
			clock_hours = (uint16_t)(10*(temp[0] - 0x30));
		}
		sendString(temp);
		// lower hour nibble
		temp[0] = 0x30+(char)HOURS_LOW(read_buffer[HOURS_REGISTER]);
		sendString(temp);
		clock_hours += (uint16_t)(1*(temp[0] - 0x30));
		// Minutes
		sendString(":");
		temp[0] = 0x30+(char)MINUTES_HIGH(read_buffer[MINUTES_REGISTER]);
		sendString(temp);
		clock_minutes = (uint16_t)(10*(temp[0] - 0x30));
		temp[0] = 0x30+(char)MINUTES_LOW(read_buffer[MINUTES_REGISTER]);
		sendString(temp);
		clock_minutes += (uint16_t)(1*(temp[0] - 0x30));
		// Seconds
		sendString(":");
		temp[0] = 0x30+(char)SECONDS_HIGH(read_buffer[SECONDS_REGISTER]);
		sendString (temp);
		temp[0] = 0x30+(char)SECONDS_LOW(read_buffer[SECONDS_REGISTER]);
		sendString(temp);
		//Print AM or PM if applicable
		if (flag_24) {
			if (flag_AM_PM)
				sendString(" PM");
			else
				sendString(" AM");
		}
		sendString(".");

		//DAY
		switch (DAYS(read_buffer[DAYS_REGISTER])) {
			case 1:
				sendString(" DAY is: Monday.");
				break;
			case 2:
				sendString(" DAY is: Tuesday.");
				break;
			case 3:
				sendString(" DAY is: Wednesday.");
				break;
			case 4:
				sendString(" DAY is: Thursday.");
				break;
			case 5:
				sendString(" DAY is: Friday.");
				break;
			case 6:
				sendString(" DAY is: Saturday.");
				break;
			case 7:
				sendString(" DAY is: Sunday.");
				break;
			default:
				break;
		}

		//DATE
		sendString(" Date is: ");
		// Day
		temp[0] = 0x30+(char)DATE_HIGH(read_buffer[DATE_REGISTER]),
		sendString(temp);
		temp[0] = 0x30+(char)DATE_LOW(read_buffer[DATE_REGISTER]);
		sendString(temp);
		// Month
		sendString("-");
		temp[0] = 0x30+(char)MONTHS_HIGH(read_buffer[MONTH_REGISTER]),
		sendString (temp);
		temp[0] = 0x30+(char)MONTHS_LOW(read_buffer[MONTH_REGISTER]);
		sendString (temp);
		// Year
		sendString("-");
		temp[0] = 0x30+(char)YEARS_HIGH(read_buffer[YEAR_REGISTER]),
		sendString(temp);
		temp[0] = 0x30+(char)YEARS_LOW(read_buffer[YEAR_REGISTER]);
		sendString(temp);
		sendString(".");
	}
}
