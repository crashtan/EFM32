/*
 * i2c.c
 *
 *  Created on: Feb 13, 2018
 *      Author: Administrator
 */

#include <stdint.h>

#include "bsp.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"

#include "i2c.h"
#include "timer.h"

 /*****************************************************************************/

I2C_TransferReturn_TypeDef I2C_Status;

/*****************************************************************************/

void setup_I2C(void) {

  /* Initialize I2C driver using standard rate. */

  I2C_Init_TypeDef i2cInit =
  {
      .enable = true,                             	// enable I2C
      .master = true,                             	// I2C master
      .refFreq = 0,                           		// measure reference clock
      .freq = 92000,                // 92khz rate
      .clhr = i2cClockHLRStandard,                	// use 4:4 bit ratio
    };

  CMU_ClockEnable(cmuClock_I2C1, true);

  /* Use location 0: SDA - PC4, SCL - PC5 */
  /* Output value must be set to 1 to not drive lines low... We set */
  /* SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(gpioPortC, 5, gpioModeWiredAndPullUpFilter, 1);	// SCL
  GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUpFilter, 1);	// SDA

  /* Enable pins at location 1 */
  I2C1->ROUTE = I2C_ROUTE_SDAPEN |
                I2C_ROUTE_SCLPEN |
				I2C_ROUTE_LOCATION_LOC0;

  I2C_Init(I2C1, &i2cInit);
  //I2C1->CTRL |= I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;

  /* Clear and enable interrupt from I2C module */
  //NVIC_ClearPendingIRQ(I2C1_IRQn);
  //NVIC_EnableIRQ(I2C1_IRQn);

  /* Setting up to enable slave mode
  I2C1->SADDR = I2C_ADDRESS;
  I2C1->CTRL |= I2C_CTRL_SLAVE | I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
  enableI2cSlaveInterrupts(); */
}

/**************************************************************************//**
 * @brief  I2C1 interrupt Handler
 *****************************************************************************/

void I2C1_IRQHandler(void) {
	I2C_Status = I2C_Transfer(I2C1);

	/* USE IF SLAVE can interrupt

	if (I2C_IF_RXDATAV){
		// Data received
		i2c_rxBuffer[i2c_rxBufferIndex] = I2C1->RXDATA;
		i2c_rxBufferIndex++;
	}
	  if (I2C_IEN_SSTOP) {
		// Stop received, reception is ended
		I2C_IntClear(I2C1, I2C_IEN_SSTOP);
		i2c_rxBufferIndex = 0;
	  }
	  */
}



/**************************************************************************//**
 * @brief  enables I2C slave interrupts
 *****************************************************************************/
void enableI2cSlaveInterrupts(void){
  I2C_IntClear(I2C1, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  I2C_IntEnable(I2C1, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  NVIC_EnableIRQ(I2C1_IRQn);
}



/**************************************************************************//**
 * @brief  disables I2C interrupts
 *****************************************************************************/
void disableI2cInterrupts(void){
  NVIC_DisableIRQ(I2C1_IRQn);
  I2C_IntDisable(I2C1, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  I2C_IntClear(I2C1, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
}

/**************************************************************************//**
 * @brief write Data to Slave
 *****************************************************************************/

void i2c_writeData(uint8_t address, uint8_t word_address, uint8_t* write_buffer, uint8_t write_length) {
	//disableI2cInterrupts();

	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr          = (address << 1);
	i2cTransfer.flags         = I2C_FLAG_WRITE_WRITE;
	i2cTransfer.buf[0].data   = &word_address;
	i2cTransfer.buf[0].len    = I2C_WORD_ADDRESS_LENGTH;
	i2cTransfer.buf[1].data   = write_buffer;
	i2cTransfer.buf[1].len    = write_length;

	I2C_TransferInit(I2C1, &i2cTransfer);
	/* Sending data */
	while (I2C_Transfer(I2C1) == i2cTransferInProgress){;}

	//enableI2cSlaveInterrupts();
}

/**************************************************************************//**
 * @brief Read Data from Slave
 *****************************************************************************/

void i2c_readData(uint8_t address, uint8_t word_address, uint8_t* read_buffer, uint8_t read_length) {
	//disableI2cInterrupts();

	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr          = (address << 1);
	i2cTransfer.flags         = I2C_FLAG_WRITE_READ;
	i2cTransfer.buf[0].data   = &word_address;
	i2cTransfer.buf[0].len    = I2C_WORD_ADDRESS_LENGTH;
	i2cTransfer.buf[1].data   = read_buffer;
	i2cTransfer.buf[1].len    = read_length;

	I2C_TransferInit(I2C1, &i2cTransfer);
	/* Reading data */
	while (I2C_Transfer(I2C1) == i2cTransferInProgress){;}

	//enableI2cSlaveInterrupts();
}
