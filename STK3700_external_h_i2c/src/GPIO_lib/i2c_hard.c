
/* i2c_hard.c
 *
 *  Created on: Feb 15, 2018
 *      Author: Administrator
 */

/***************************************************************************//**
 * @brief HEADERS AND INCLUDES. CHECK HEADER FILE FOR PINOUTS.
 ******************************************************************************/
//EXTERNAL
#include <string.h>

//GPIO_LIB
#include "timer.h"
#include "em_gpio.h"

//HEADER
#include "i2c_hard.h"

/***************************************************************************//**
 * @brief GLOBAL VARIABLES.
 ******************************************************************************/
uint8_t uint8_t_temp_byte;


/***************************************************************************//**
 * @brief SCL AND SDA PIN CONFIGURATION FUNCTIONS. ALSO A CLOCK TEST FUNCTION.
 ******************************************************************************/

//PF9 is SCL
static void SCL_configure(mode enum_mode_high_low) {
	if (enum_mode_high_low == HIGH) {
		delayInMS(SHORT_DELAY);
		GPIO_PinModeSet(gpioPortF, SCL_PIN, gpioModePushPull, 1);
		delayInMS(SHORT_DELAY);
	}
	if (enum_mode_high_low == LOW) {
		delayInMS(SHORT_DELAY);
		GPIO_PinModeSet(gpioPortF, SCL_PIN, gpioModePushPull, 0);
		delayInMS(SHORT_DELAY);
	}
}

/******************************************************************************/
//PF8 is SDA
static void SDA_configure(mode enum_mode_high_low, in_out input_output) {

	if ((enum_mode_high_low == HIGH) && (input_output == OUTPUT))
		GPIO_PinModeSet(gpioPortF, SDA_PIN, gpioModePushPull, 1);

	if ((enum_mode_high_low == LOW) && (input_output == OUTPUT))
		GPIO_PinModeSet(gpioPortF, SDA_PIN, gpioModePushPull, 0);

	if ((enum_mode_high_low == HIGH) && (input_output == INPUT))
		GPIO_PinModeSet(gpioPortF, SDA_PIN, gpioModeInput, 0);

	if ((enum_mode_high_low == LOW) && (input_output == INPUT))
		GPIO_PinModeSet(gpioPortF, SDA_PIN, gpioModeInput, 0);
}

/******************************************************************************/

//Clock toggle test
static void clockToggle (uint8_t uint8_t_times) {
	for (uint8_t i = 0; i<uint8_t_times; i++) {
		SCL_configure(LOW);
		delayInMS(SHORT_DELAY);
		SCL_configure(HIGH);
		delayInMS(LONG_DELAY);
		SCL_configure(LOW);
		delayInMS(SHORT_DELAY);
	}
}

/***************************************************************************//**
 * @brief START AND STOP FUNCTIONS
 ******************************************************************************/
static void start_signal(void) {

	//Set SDA Low
	SCL_configure(HIGH);
	SDA_configure(HIGH,OUTPUT);
	SDA_configure(LOW, OUTPUT);
	SCL_configure(LOW);
}

/******************************************************************************/

static void stop_signal(void){
	SCL_configure(LOW);
	SDA_configure(LOW,OUTPUT);

	SCL_configure(HIGH);
	SDA_configure(HIGH, OUTPUT);
	delayInMS(LONG_DELAY);
	SCL_configure(LOW);
}

/***************************************************************************//**
 * @brief ACK FUNCTION. NACK FUNCTION IS IMPLEMENTED AS SEND_BIT.
 ******************************************************************************/

static uint8_t wait_for_ack(void) {
	uint8_t uint8_t_ack_bit = 0;

	// Send ACK bit
	SDA_configure(LOW, INPUT);

	//delayInMS(SHORT_DELAY);
	SCL_configure(HIGH);
	delayInMS(LONG_DELAY);

	uint8_t_ack_bit = GPIO_PinInGet(gpioPortF, SDA_PIN);

	SCL_configure(LOW);

	return uint8_t_ack_bit;
}

/***************************************************************************//**
 * @brief SEND_BIT FUNCTION USED IN SEND_BYTE. ALSO USED AS NACK.
 ******************************************************************************/

static void send_bit(bit_type type) {
	if (type == ONE) {
		SDA_configure(HIGH, OUTPUT);
		SCL_configure(HIGH);

		delayInMS(LONG_DELAY);

		SCL_configure(LOW);
		SDA_configure(LOW, OUTPUT);
	}
	else {
		SDA_configure(LOW, OUTPUT);
		SCL_configure(HIGH);

		delayInMS(LONG_DELAY);

		SCL_configure(LOW);
		SDA_configure(LOW, OUTPUT);
	}
}

/***************************************************************************//**
 * @brief SEND OUTPUT BYTE, SEND ADDRESS, RECEIVE BYTE.
 ******************************************************************************/

static void send_byte(uint8_t byte) {
	//send uint8_t_address and r/w signal
	for (uint8_t i=0; i<8; i++) {
		if ((byte & 0x80) == 0 )
			send_bit(ZERO);
		else
			send_bit(ONE);
		byte<<=1;
	}
}

/******************************************************************************/

static void send_address(uint8_t uint8_t_address, bool r_w_enable) {
	// READ BIT
	if (r_w_enable) {
		uint8_t_address = (uint8_t_address<<1) | 0x01;
		send_byte(uint8_t_address);
	}
	else {
		uint8_t_address = (uint8_t_address<<1) | 0x00;
		send_byte(uint8_t_address);
	}
}

/******************************************************************************/

static void receive_byte(void) {
	uint8_t_temp_byte = 0;
	uint8_t temp_bit;

	for (uint8_t i=8; i>0;i--) {

		SDA_configure(LOW, INPUT);

		SCL_configure(HIGH);
		//delayInMS(SHORT_DELAY);

		temp_bit = GPIO_PinInGet(gpioPortF, SDA_PIN);
		uint8_t_temp_byte |= (temp_bit<<(i-1));

		//delayInMS(SHORT_DELAY);
		SCL_configure(LOW);

	}
}

/***************************************************************************//**
 * @brief: I2C FUNCTIONS: DATA READ, DATA WRITE, DATA READ WRITE, DATA WRITE WRITE.
 ******************************************************************************/

void read_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint16_t uint16_t_data_length) {
	uint8_t uint8_t_ack_bit;

	for(uint8_t i=0; i<uint16_t_data_length; i++) {

		//SEND START
		start_signal();

		//RESEND uint8_t_address WITH R bit
		send_address(uint8_t_address, true);
		uint8_t_ack_bit = wait_for_ack();
		if (uint8_t_ack_bit)
			stop_signal();

		//READ DATA
		receive_byte();
		uint8_t_buffer[i] = uint8_t_temp_byte;
		uint8_t_ack_bit = wait_for_ack();

		if (i<uint16_t_data_length-1) {
			//SEND ACK
			uint8_t_ack_bit = wait_for_ack();
			if (uint8_t_ack_bit){
				stop_signal();
			}
		} else {
			//SEND NACK
			send_bit(ONE);
			//SEND STOP
			stop_signal();
		}
	}
}

/******************************************************************************/

void write_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint16_t uint16_t_data_length)  {
	uint8_t uint8_t_ack_bit;

	//SEND START
	start_signal();

	//SEND uint8_t_address WITH W bit
	send_address(uint8_t_address, false);
	uint8_t_ack_bit = wait_for_ack();
	if (uint8_t_ack_bit)
		stop_signal();

	//SEND DATA
	for(uint8_t i=0; i<=uint16_t_data_length; i++) {
		send_byte(uint8_t_buffer[i]);

		//SEND ACK
		uint8_t_ack_bit = wait_for_ack();
		if (uint8_t_ack_bit)
			stop_signal();
	}
	//STOP SIGNAL
	stop_signal();
}

/******************************************************************************/

void write_read_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint8_t uint8_t_word_address, uint16_t uint16_t_data_length) {
	uint8_t uint8_t_ack_bit;

	//SEND START
	start_signal();

	//SEND uint8_t_address WITH W bit
	send_address(uint8_t_address, false);
	uint8_t_ack_bit = wait_for_ack();
	if (uint8_t_ack_bit)
		stop_signal();

	//SEND WORD uint8_t_address
	send_byte(uint8_t_word_address);
	uint8_t_ack_bit = wait_for_ack();
	if (uint8_t_ack_bit)
		stop_signal();

	for(uint8_t i=0; i<uint16_t_data_length; i++) {

		//SEND START
		start_signal();

		//RESEND uint8_t_address WITH R bit
		send_address(uint8_t_address, true);
		uint8_t_ack_bit = wait_for_ack();
		if (uint8_t_ack_bit)
			stop_signal();

		//READ DATA
		receive_byte();
		uint8_t_buffer[i] = uint8_t_temp_byte;
		uint8_t_ack_bit = wait_for_ack();

		if (i<uint16_t_data_length-1) {
			//SEND ACK
			uint8_t_ack_bit = wait_for_ack();
			if (uint8_t_ack_bit){
				stop_signal();
			}
		} else {
			//SEND NACK
			send_bit(ONE);
			//SEND STOP
			stop_signal();
		}
	}
}

/******************************************************************************/

void write_write_data(uint8_t* uint8_t_buffer, uint8_t uint8_t_address, uint8_t uint8_t_word_address, uint16_t uint16_t_data_length)  {
	uint8_t uint8_t_ack_bit;

	//SEND START
	start_signal();

	//SEND uint8_t_address WITH W bit
	send_address(uint8_t_address, false);
	uint8_t_ack_bit = wait_for_ack();
	if (uint8_t_ack_bit)
		stop_signal();

	//SEND WORD uint8_t_address
	send_byte(uint8_t_word_address);
	uint8_t_ack_bit = wait_for_ack();
	if (uint8_t_ack_bit)
		stop_signal();

	//SEND DATA
	for(uint8_t i=0; i<=uint16_t_data_length; i++) {
		send_byte(uint8_t_buffer[i]);

		//SEND ACK
		uint8_t_ack_bit = wait_for_ack();
		if (uint8_t_ack_bit)
			stop_signal();
	}
	//STOP SIGNAL
	stop_signal();
}


/***************************************************************************//**
 * @brief INITIALIZE I2C AND TIMERS. NEED TO INCLUDE "GPIO_lib/timer.h".
 ******************************************************************************/
void setup_hard_i2c(void) {
	// Initialize Timer as CLock source
	setTimerType (PSCALE_1, MHZ_11);		//Timer frequency and prescaling
	setupTimers(TIMER_0, 1000);				//Timer top time
	INTR_EN_timer(TIMER_0);					//Enable Timer interrupt on overflow
	startTimer(TIMER_0);					//start timer
	milliCounter = 0;

	// Set SCL High
	SCL_configure(HIGH);

	// Set DATA High
	SDA_configure(HIGH, OUTPUT);

}



/***************************************************************************//**
 * @brief FUNCTION FOR TESTING OTHER FUNCTIONS. CHECK OUTPUT USING SCOPE.
 ******************************************************************************/

void I2C_hard_test(uint8_t uint8_t_test_address, uint8_t* uint8_t_input_buffer, uint8_t* uint8_t_output_buffer) {
	//send_bit(ONE);

	//send_address(DS1307_ADDRESS, false);

	/*
	start_signal();

	send_bit(ONE);
	send_bit(ONE);
	send_bit(ZERO);
	send_bit(ONE);
	send_bit(ZERO);
	send_bit(ZERO);
	send_bit(ZERO);
	send_bit(ONE);

	wait_for_ack();

	stop_signal();
	*/

	//start_signal();
	//stop_signal();

	/*
	send_byte(0b00000001);
	wait_for_ack();
	delayInMS(500);
	*/

	write_write_data(uint8_t_input_buffer, uint8_t_test_address, 0x00, 10);
	write_read_data(uint8_t_output_buffer, uint8_t_test_address, 0x00, 10);

}

/******************************************************************************/



