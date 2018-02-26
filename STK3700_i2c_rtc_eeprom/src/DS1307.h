/*
 * DS1307.h
 *
 *  Created on: Feb 23, 2018
 *      Author: Administrator
 */

#ifndef SRC_GPIO_LIB_DS1307_H_
#define SRC_GPIO_LIB_DS1307_H_

#define ATMEL_EEPROM_24C04N_ADDRESS 	0x50
#define DS1307_ADDRESS 	0x68
#define DS1307_BUFFER_SIZE 7

#define SECONDS_REGISTER	0x00
#define MINUTES_REGISTER	0x01
#define HOURS_REGISTER		0x02
#define DAYS_REGISTER		0x03
#define DATE_REGISTER		0x04
#define MONTH_REGISTER		0x05
#define YEAR_REGISTER		0x06
#define CNTRL_REGISTER		0x07

#define ENABLE_BIT(x)	( (x & 0b10000000)>>7 )
#define SECONDS_LOW(x)	( x & 0b00001111 )
#define SECONDS_HIGH(x)	( (x & 0b01110000)>>4 )
#define MINUTES_LOW(x)	( x & 0b00001111 )
#define MINUTES_HIGH(x)	( (x & 0b01110000)>>4 )
#define HOURS_LOW(x)	( x & 0b00001111 )
#define HOURS_HIGH(x)	( (x & 0b01110000)>>4 )
#define DAYS(x)			( x & 0b00000111 )
#define DATE_LOW(x)		( x & 0b00001111 )
#define DATE_HIGH(x)	( (x & 0b00110000)>>4 )
#define MONTHS_LOW(x)	( x & 0b00001111 )
#define MONTHS_HIGH(x)	( (x & 0b00010000)>>4 )
#define YEARS_LOW(x)	( x & 0b00001111 )
#define YEARS_HIGH(x)	( (x & 0b11110000)>>4 )

extern uint8_t read_buffer[DS1307_BUFFER_SIZE];
extern uint8_t write_buffer[DS1307_BUFFER_SIZE];

extern uint16_t clock_hours;
extern uint16_t clock_minutes;

typedef enum WEEKDAY {
	MONDAY 		= 1,
	TUESDAY 	= 2,
	WEDNESDAY 	= 3,
	THURSDAY 	= 4,
	FRIDAY 		= 5,
	SATURDAY 	= 6,
	SUNDAY 		= 7,
} weekday;

typedef enum DAY_NIGHT {
	AM,
	PM,
} day_night;

typedef enum CLOCK_TYPE {
	_12_HOUR,
	_24_HOUR,
} clock_type;

void write_to_DS1307(void);
void print_DS1307(void);
void update_DS1307_from_EEPROM(void);
void update_DS1307(void);

void DS1307_set_day(weekday day_name);
void DS1307_set_date(char* string);
void DS1307_set_month(char* string);
void DS1307_set_year(char* string);

void DS1307_set_hour(char* string, day_night am_pm, clock_type _24_hr);
void DS1307_set_minute(char* string);
void DS1307_set_second(char* string);

void enable_DS1307_clock(bool enable);


#endif /* SRC_GPIO_LIB_DS1307_H_ */
