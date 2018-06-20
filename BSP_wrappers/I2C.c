/*
 * I2C.c
 *
 *  Created on: Jun 1, 2018
 *      Author: moiz.hussain
 */
#include "I2C.h"

void I2C1_Setup (void){
	/* Enabling clock to the I2C, GPIO */
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_I2C1, true);

	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	/* Using PC4 (SDA) and PC5 (SCL) */
	GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortC, 5, gpioModeWiredAndPullUpFilter, 1);

	/* Enable pins at location 1 */
	I2C1->ROUTE = I2C_ROUTE_SDAPEN |
				I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC0;

	i2cInit.freq = I2C_DEFAULT_FREQ;

	/* Initializing the I2C */
	I2C_Init(I2C1, &i2cInit);
}

void PerformI2C_READ(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *ReadBuffer, uint16_t Length) {
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr          = SlaveAddress<<1;
	i2cTransfer.buf[0].data   = &WriteRegisterAddress;
	i2cTransfer.buf[0].len    = 1;

	// Select location/length of data to be read
	i2cTransfer.buf[1].data = ReadBuffer;
	i2cTransfer.buf[1].len = Length;

	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	I2C_TransferInit(I2C1, &i2cTransfer);

	/* Transmit/receive data */
	while (I2C_Transfer(I2C1) == i2cTransferInProgress);
}

void PerformI2C_WRITE(uint16_t SlaveAddress, uint8_t WriteRegisterAddress, uint8_t *WriteBuffer, uint16_t Length){
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Initializing I2C transfer */
	i2cTransfer.addr          = SlaveAddress<<1;
	i2cTransfer.buf[0].data   = &WriteRegisterAddress;
	i2cTransfer.buf[0].len    = 1;

	// Select location/length of data to be read
	i2cTransfer.buf[1].data = WriteBuffer;
	i2cTransfer.buf[1].len = Length;

	/* Master write */
	i2cTransfer.flags = I2C_FLAG_WRITE_WRITE;

	I2C_TransferInit(I2C1, &i2cTransfer);

	/* Transmit/receive data */
	while (I2C_Transfer(I2C1) == i2cTransferInProgress);
}
