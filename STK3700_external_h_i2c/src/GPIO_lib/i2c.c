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

uint8_t i2c_txBuffer[I2C_BUFFER_SIZE];
uint8_t i2c_rxBuffer[I2C_BUFFER_SIZE];
uint8_t i2c_rxBufferIndex;

I2C_TransferReturn_TypeDef I2C_Status;

/*****************************************************************************/

void setup_I2C(void) {

  /* Initialize I2C driver using standard rate. */

  I2C_Init_TypeDef i2cInit =
  {
      .enable = true,                              // enable I2C
      .master = true,                              // I2C master
      .refFreq = false,                                // measure reference clock
      .freq = 50000,                              	// 1k rate
      .clhr = i2cClockHLRStandard,                 // use 6:3 bit ratio
    };

  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_I2C1, true);

  /* Use location 3: SDA - PC4, SCL - PC5 */
  /* Output value must be set to 1 to not drive lines low... We set */
  /* SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(gpioPortC, 5, gpioModeWiredAndPullUpFilter, 1);	// SCL
  GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUpFilter, 1);	// SDA

  /* Enable pins at location 1 */
  I2C1->ROUTE = I2C_ROUTE_SDAPEN |
                I2C_ROUTE_SCLPEN |
                (1 << _I2C_ROUTE_LOCATION_SHIFT);

  I2C_Init(I2C1, &i2cInit);
  I2C1->CTRL |= I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;

  i2c_rxBufferIndex = 0;

  /* Clear and enable interrupt from I2C module */
  NVIC_ClearPendingIRQ(I2C1_IRQn);
  NVIC_EnableIRQ(I2C1_IRQn);

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

void i2c_writeData(uint8_t address) {
	//disableI2cInterrupts();

	  /* Transfer structure */
	  I2C_TransferSeq_TypeDef i2cTransfer;


	  /* Initializing I2C transfer */
	  i2cTransfer.addr          = (EEPROM_ADDRESS << 1 );
	  i2cTransfer.flags         = I2C_FLAG_WRITE;
	  i2cTransfer.buf[0].data   = &address;
	  i2cTransfer.buf[0].len    = I2C_BUFFER_SIZE;
	  i2cTransfer.buf[1].data   = i2c_rxBuffer;
	  i2cTransfer.buf[1].len    = I2C_BUFFER_SIZE;

	  I2C_TransferInit(I2C1, &i2cTransfer);
	  I2C_TransferReturn_TypeDef ret_test = I2C_Transfer(I2C1);
	  /* Sending data */
	  while ( ret_test == i2cTransferInProgress){
		  ret_test = I2C_Transfer(I2C1);
	  }

	  //enableI2cSlaveInterrupts();
}

/**************************************************************************//**
 * @brief Read Data from Slave
 *****************************************************************************/

void i2c_readData(uint8_t address) {
	//disableI2cInterrupts();

		uint8_t add = address;
	  /* Transfer structure */
	  I2C_TransferSeq_TypeDef i2cTransfer;


	  /* Initializing I2C transfer */
	  i2cTransfer.addr          = (I2C_ADDRESS_DS1307<<1);
	  i2cTransfer.flags         = I2C_FLAG_READ;
	  i2cTransfer.buf[0].data   = i2c_rxBuffer;
	  i2cTransfer.buf[0].len    = I2C_BUFFER_SIZE;
	  //i2cTransfer.buf[1].data   = i2c_rxBuffer;
	  //i2cTransfer.buf[1].len    = I2C_BUFFER_SIZE;

	  I2C_TransferInit(I2C1, &i2cTransfer);
	  I2C_TransferReturn_TypeDef ret_test;
	  /* Reading data */
	  while (I2C_Transfer(I2C1) == i2cTransferInProgress){;}

	 //enableI2cSlaveInterrupts();
}
