#include "i2c.h"

/**
 * I2CInit: Initialize the I2C communication
 */

void initI2C()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	/* Specify clock for I2C */
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	/* Enable peripherals */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Initialize SCL and SDA pins */
	GPIO_PinAFConfig(GPIOB, PINSOURCE_I2C_SCL, GPIO_AF_4); // SCL PB8
	GPIO_PinAFConfig(GPIOB, PINSOURCE_I2C_SDA, GPIO_AF_4); // SDA PB9

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD; // set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;   // enable pull up resistors

	GPIO_InitStruct.GPIO_Pin = PIN_I2C_SCL | PIN_I2C_SDA; //SDA
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure I2C */
	// Disable I2C1 - PE bit in I2C_CR1
	I2C_Cmd(I2C1, DISABLE);
	I2C_DeInit(I2C1);

	// configure I2C1
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; // I2C mode
	I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStruct.I2C_DigitalFilter = 0;
	I2C_InitStruct.I2C_OwnAddress1 = 0;									   // own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;							   // disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_InitStruct.I2C_Timing = 0x10805E89;								   //0x20000909; // 100kHz  0x10805E89    Meins: 0x00201D2B
	I2C_Init(I2C1, &I2C_InitStruct);									   // init I2C1

	I2C_DualAddressCmd(I2C1, DISABLE);
	I2C_GeneralCallCmd(I2C1, DISABLE);
	I2C_StretchClockCmd(I2C1, ENABLE);

	// enable I2C1 - PE bit in I2C_CR1
	I2C_Cmd(I2C1, ENABLE);
}

/**
 * initCD: initialize and configure coin detector
 */
void initCD()
{
	//	for(int i = 0; i<100000; i++);
	//	buffer[0] = 2;
	//	buffer[1] = 0xFF;
	//	sendData(I2C_ADD_CD, 2);

	for (int i = 0; i < 100000; i++)
	{
		// do nothing but wait before sending data
	}
	buffer[0] = 3;
	buffer[1] = 0xFF;
	sendData(I2C_ADD_CD, 2);
}

/**
 * sendData: send a packet over the I2C line
 *
 * Sends bytes contained in global variable buffer
 *
 * Parameters:
 * 	address: the slave address (note: is expected as a left-shifted 7-bit address)
 * 	nBytes: the number of bytes to transmit
 */

void sendData(uint8_t address, uint8_t nBytes)
{
	// Wait until line not busy
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
	{
		puts("busy");
	};

	// Set master transmitter mode
	I2C_MasterRequestConfig(I2C1, I2C_Direction_Transmitter);
	// Start transfer
	I2C_TransferHandling(I2C1, address, nBytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);

	uint8_t i;
	for (i = 0; i < nBytes; i++)
	{
		// Wait until TXIS is set
		while (I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
			;

		// Write byte
		I2C_SendData(I2C1, buffer[i]);
	}

	// Wait until stop flag is set, since AUTOEND mode is enabled
	while (I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) != SET)
		;
	I2C_ClearFlag(I2C1, I2C_ISR_STOPF);

	// Reset CR2 register
	I2C1->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}

/**
 * receiveData: receive a packet over the I2C line
 *
 * Stores received bytes into global variable buffer
 *
 * Parameters:
 * 	address: the slave address (note: is expected as a left-shifted 7-bit address)
 * 	nBytes: the number of bytes to receive
 */

void receiveData(uint8_t address, uint8_t nBytes)
{
	// Wait until line not busy
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
		;
	// Set master receiver mode
	I2C_MasterRequestConfig(I2C1, I2C_Direction_Receiver);

	uint8_t count = nBytes;
	// Start transfer
	I2C_TransferHandling(I2C1, address, nBytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	while (count > 0)
	{
		// Wait until RXNE flag is set
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) != SET)
			;
		buffer[nBytes - count] = I2C_ReceiveData(I2C1);

		count--;
	}

	// Wait until stop flag is set, since AUTOEND mode is enabled
	while (I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) != SET)
		;
	I2C_ClearFlag(I2C1, I2C_ISR_STOPF);

	// Reset CR2 register
	I2C1->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}

/**
 * sendReceiveData: send a packet over the I2C line, then receive a packet back
 *
 * Sends bytes contained in global variable buffer
 * Then store received bytes into global variable buffer
 *
 * Parameters:
 * 	address: the slave address (note: is expected as a left-shifted 7-bit address)
 * 	nBytesSend: the number of bytes to transmit
 * 	nBytesReceive: the number of bytes to receive
 */

void sendReceiveData(uint8_t address, uint8_t nBytesSend, uint8_t nBytesReceive)
{
	// Wait until line not busy
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET)
		;
	// Set master transmitter mode
	I2C_MasterRequestConfig(I2C1, I2C_Direction_Transmitter);
	// Start transfer
	I2C_TransferHandling(I2C1, address, nBytesSend, I2C_Reload_Mode, I2C_Generate_Start_Write);

	uint8_t i;
	for (i = 0; i < nBytesSend; i++)
	{
		// Wait until TXIS is set
		while (I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
			;

		// Write byte
		I2C_SendData(I2C1, buffer[i]);
	}

	// Set master receiver mode
	I2C_MasterRequestConfig(I2C1, I2C_Direction_Receiver);

	uint8_t count = nBytesReceive;
	// Start transfer
	I2C_TransferHandling(I2C1, address, nBytesReceive, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	while (count > 0)
	{
		// Wait until RXNE flag is set
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) != SET)
			;
		buffer[nBytesReceive - count] = I2C_ReceiveData(I2C1);

		count--;
	}

	// Wait until stop flag is set, since AUTOEND mode is enabled
	while (I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) != SET)
		;
	I2C_ClearFlag(I2C1, I2C_ISR_STOPF);

	// Reset CR2 register
	I2C1->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN));
}
