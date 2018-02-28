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
char date_string[8];

/*******************************************************************************************/

inline static char to_uint16_t (char character) {
	return (uint16_t)(character-0x30);
}

/*******************************************************************************************/

inline static char toChar (uint8_t value) {
	return (char)(value+0x30);
}

/*******************************************************************************************/


inline static char to_uint8_t (char character) {
	return (uint8_t)(character-0x30);
}

/*******************************************************************************************/

uint16_t DS1307_getHours(void) {
	return clock_hours;
}

/*******************************************************************************************/

uint16_t DS1307_getMinutes(void) {
	return clock_minutes;
}

/*******************************************************************************************/

char* DS1307_getDate(void) {

	date_string[0] = toChar(DATE_HIGH(read_buffer[DATE_REGISTER]));
	date_string[1] = toChar(DATE_LOW(read_buffer[DATE_REGISTER]));
	date_string[2] = toChar(MONTHS_HIGH(read_buffer[MONTH_REGISTER]));
	date_string[3] = toChar(MONTHS_LOW(read_buffer[MONTH_REGISTER]));
	date_string[4] = toChar(YEARS_HIGH(read_buffer[YEAR_REGISTER]));
	date_string[5] = toChar(YEARS_LOW(read_buffer[YEAR_REGISTER]));

	uint8_t temp = DAYS(read_buffer[DAYS_REGISTER]);

	switch (temp) {
	case 1:
		date_string[6] = 'M';
		break;
	case 2:
		date_string[6] = 'T';
		break;
	case 3:
		date_string[6] = 'W';
		break;
	case 4:
		date_string[6] = 'T';
		break;
	case 5:
		date_string[6] = 'F';
		break;
	case 6:
		date_string[6] = 'S';
		break;
	case 7:
		date_string[6] = 'S';
		break;
	default:
		date_string[6] = '\0';
		break;
	}

	date_string[7] = '\0';

	return date_string;
}


/*******************************************************************************************/

void DS1307_setMode(void) {
	enable_DS1307_clock(true);
	update_DS1307();

	DAY:
	sendString("\r\nMonday 		= 1\r\nTuesday 	= 2\r\nWednesday 	= 3\r\nThursday 	= 4\r\nFriday 		= 5\r\nSaturday 	= 6\r\nSunday 		= 7\r\nEnter 1 digit day value and press Enter: ");
	waitForInput();
	if (!DS1307_set_day(currentString) )
		goto DAY;

	DATE:
	sendString("\r\nEnter 2 digit date value(01-31) and press Enter: ");
	waitForInput();
	if (!DS1307_set_date(currentString) )
		goto DATE;

	MONTH:
	sendString("\r\nEnter 2 digit month value(01-12) and press Enter: ");
	waitForInput();
	if (!DS1307_set_month(currentString) )
		goto MONTH;

	YEAR:
	sendString("\r\nEnter 2 digit year value(00-99) and press Enter: ");
	waitForInput();
	if (!DS1307_set_year(currentString) )
		goto YEAR;

	HOUR:
	sendString("\r\n12 Hour = 1\r\n24 Hour = 2\r\nChoose Clock type, 12 or 24hr and press Enter: ");
	waitForInput();
	//12 HOUR CHOSEN
	if ((currentString[0] =='1') && (strlen(currentString)==1)){
		sendString("\r\nAM = 1\r\nPM = 2\r\nChoose day time, AM or PM and press Enter: ");
		waitForInput();
		if ((currentString[0] =='1') && (strlen(currentString)==1)) {
			//AM CHOSEN
			sendString("\r\nEnter 2 digit hour value(00-12) and press Enter: ");
			waitForInput();
			if (!DS1307_set_hour(currentString, AM, _12_HOUR) ) {
				goto HOUR;
			}
		} else {
			if ((currentString[0] =='2') && (strlen(currentString)==1)) {
				//PM CHOSEN
				sendString("\r\nEnter 2 digit hour value(00-12) and press Enter: ");
				waitForInput();
				if (!DS1307_set_hour(currentString, PM, _12_HOUR) )
					goto HOUR;
			} else {
				// OTHER THAN 1 OR 2
				sendString("Illegal Choice Entered, Re-enter Hour Data!");
				goto HOUR;
			}
		}
	} else {
		//24 HOUR CHOSEN
		if ((currentString[0] =='2') && (strlen(currentString)==1)){
			sendString("\r\nEnter 2 digit hour value(00-23) and press Enter: ");
			waitForInput();
			if (!DS1307_set_hour(currentString, AM, _24_HOUR) )
				goto HOUR;
		} else {
			// OTHER THAN 1 OR 2
			sendString("Illegal Choice Entered, Re-enter Hour Data!");
			goto HOUR;
		}
	}

	MINUTES:
	sendString("\r\nEnter 2 digit minutes value(00-59) and press Enter: ");
	waitForInput();
	if (!DS1307_set_minute(currentString) )
		goto MINUTES;

	SECONDS:
	sendString("\r\nEnter 2 digit seconds value(00-59) and press Enter: ");
	waitForInput();
	if (!DS1307_set_second(currentString) )
		goto SECONDS;

}

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

bool DS1307_set_day(char* string){

	if (strlen(string) !=1) {
		sendString("\r\nIllegal Choice Entered!");
		return false;
	}

	uint8_t day_to_int = 0;
	char temp = string[0];

	switch (temp) {
	case '1':
		day_to_int = 1;
		break;
	case '2':
		day_to_int = 2;
		break;
	case '3':
		day_to_int = 3;
		break;
	case '4':
		day_to_int = 4;
		break;
	case '5':
		day_to_int = 5;
		break;
	case '6':
		day_to_int = 6;
		break;
	case '7':
		day_to_int = 7;
		break;
	default:
		sendString("\r\nIllegal Choice Entered!");
		break;
	}

	if ((day_to_int >0) && (day_to_int <8)){
		write_buffer[DAYS_REGISTER] = (uint8_t)day_to_int;
		write_to_DS1307();
		return true;
	}
	return false;
}


bool DS1307_set_date(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Date Input, Enter 2 digits Only!");
		return false;
	} else {
		if ( (string[0] < '0') || (string[0] >'3') || (string[1] < '0') || (string[0] >'9') ) {
			sendString("\n\rEntry out of range.");
			return false;
		} else {
			if ((string[0] =='3') && (string[1] >'1')) {
				sendString("\n\rEntry out of range.");
				return false;
			}
			if ((string[0] =='0') && (string[1] <'1')) {
				sendString("\n\rEntry out of range.");
				return false;
			}
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
			uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
			write_buffer[DATE_REGISTER] = lower|upper;
			write_to_DS1307();
			return true;
		}
	}
}


bool DS1307_set_month(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Months Input, Enter 2 digits Only!");
		return false;
	} else {
		if ( (string[0] < '0') || (string[0] >'1') || (string[1] < '0') || (string[0] >'9') ) {
			sendString("\n\rEntry out of range.");
			return false;
		} else {
			if ((string[0] =='1') && (string[1] >'2')) {
				sendString("\n\rEntry out of range.");
				return false;
			}
			if ((string[0] =='0') && (string[1] <'1')) {
				sendString("\n\rEntry out of range.");
				return false;
			}
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
			uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
			write_buffer[MONTH_REGISTER] = lower|upper;
			write_to_DS1307();
			return true;
		}
	}
}


bool DS1307_set_year(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Year Input, Enter 2 digits Only!");
		return false;
	} else {
		if ( (string[0] < '0') || (string[0] >'9') || (string[1] < '0') || (string[0] >'9') ) {
			sendString("\n\rEntry out of range.");
			return false;
		} else {
			//SET THE YEAR
			uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
			uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
			write_buffer[YEAR_REGISTER] = lower|upper;
			write_to_DS1307();
			return true;
		}
	}
}

/*******************************************************************************************/

bool DS1307_set_hour(char* string, day_night am_pm, clock_type _24_hr){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Hours Input, Enter 2 digits Only!");
		return false;
	} else {
		if (_24_hr == _24_HOUR) {
			if ( (string[0] < '0') || (string[0] >'2') || (string[1] < '0') || (string[0] >'9') ) {
				sendString("\n\rEntry out of range.");
				return false;
			} else {
				if ((string[0] =='2') && (string[1] >'3')) {
					sendString("\n\rEntry out of range.");
					return false;
				}
				//SET THE HOUR
				uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
				uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
				write_buffer[HOURS_REGISTER] = 0b00000000;
				write_buffer[HOURS_REGISTER] = lower|upper;
				write_to_DS1307();
				return true;
			}
		} else {
			// NOT 24 HOUR
			if ( (string[0] < '0') || (string[0] >'1') || (string[1] < '0') || (string[0] >'9') ) {
				sendString("\n\rEntry out of range.");
				return false;
			} else {
				if ((string[0] =='1') && (string[1] >'2')) {
					sendString("\n\rEntry out of range.");
					return false;
				}
				if ((string[0] =='0') && (string[1] <'1')) {
					sendString("\n\rEntry out of range.");
					return false;
				}
				//SET THE HOUR AND AM PM
				uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
				uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
				uint8_t bit_sum = lower|upper|0b01000000;
				if (am_pm == PM)
					bit_sum |= 0b00100000;
				write_buffer[HOURS_REGISTER] = bit_sum;
				write_to_DS1307();
				return true;
			}
		}
	}
}

bool DS1307_set_minute(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Minutes Input, Enter 2 digits Only!");
		return false;
	} else {
		if ( (string[0] < '0') || (string[0] >'5') || (string[1] < '0') || (string[0] >'9') ) {
			sendString("\n\rEntry out of range.");
			return false;
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
			uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
			write_buffer[MINUTES_REGISTER] = lower|upper;
			write_to_DS1307();
			return true;
		}
	}
}

bool DS1307_set_second(char* string){
	if ((strlen(string) !=2) || (string[2] !='\0')) {
		sendString("\n\rMalformed Seconds Input, Enter 2 digits Only!");
		return false;
	} else {
		if ( (string[0] < '0') || (string[0] >'5') || (string[1] < '0') || (string[0] >'9') ) {
			sendString("\n\rEntry out of range.");
			return false;
		} else {
			//SET THE MINUTE
			uint8_t upper = (((uint8_t)(to_uint8_t(string[0])))<<4);
			uint8_t lower =	((uint8_t)(to_uint8_t(string[1])));
			write_buffer[SECONDS_REGISTER] = lower|upper;
			write_to_DS1307();
			return true;
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
				temp[0] = toChar((_24hr_check & 0b001));
				clock_hours = (uint16_t)(10*to_uint16_t(temp[0]));
		} else {
			// 24 HOUR ENABLED
			temp[0] = toChar(_24hr_check);
			clock_hours = (uint16_t)(10*to_uint16_t(temp[0]));
		}
		sendString(temp);
		// lower hour nibble
		temp[0] = toChar(HOURS_LOW(read_buffer[HOURS_REGISTER]));
		sendString(temp);
		clock_hours += (uint16_t)(1*to_uint16_t(temp[0]));
		// Minutes
		sendString(":");
		temp[0] = toChar(MINUTES_HIGH(read_buffer[MINUTES_REGISTER]));
		sendString(temp);
		clock_minutes = (uint16_t)(10*to_uint16_t(temp[0]));
		temp[0] = toChar(MINUTES_LOW(read_buffer[MINUTES_REGISTER]));
		sendString(temp);
		clock_minutes += (uint16_t)(1*to_uint16_t(temp[0]));
		// Seconds
		sendString(":");
		temp[0] = toChar(SECONDS_HIGH(read_buffer[SECONDS_REGISTER]));
		sendString (temp);
		temp[0] = toChar(SECONDS_LOW(read_buffer[SECONDS_REGISTER]));
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
		temp[0] = toChar(DATE_HIGH(read_buffer[DATE_REGISTER])),
		sendString(temp);
		temp[0] = toChar(DATE_LOW(read_buffer[DATE_REGISTER]));
		sendString(temp);
		// Month
		sendString("-");
		temp[0] = toChar(MONTHS_HIGH(read_buffer[MONTH_REGISTER])),
		sendString (temp);
		temp[0] = toChar(MONTHS_LOW(read_buffer[MONTH_REGISTER]));
		sendString (temp);
		// Year
		sendString("-");
		temp[0] = toChar(YEARS_HIGH(read_buffer[YEAR_REGISTER])),
		sendString(temp);
		temp[0] = toChar(YEARS_LOW(read_buffer[YEAR_REGISTER]));
		sendString(temp);
		sendString(".");
	}
}
