/*
 * uart.c
 *
 *  Created on: 4 Sep 2017
 *      Author: lboerefijn
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */

#include "uart.h"

/**
 * uartInit: initialize uart communication device and corresponding pins
 */
void initUART()
{
	// Enable clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	// Setup pins
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = PIN_UART_RX | PIN_UART_TX;
	gpioInit.GPIO_Mode = GPIO_Mode_AF;
	gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(PORT_UART, &gpioInit);
	GPIO_PinAFConfig(PORT_UART, PINSOURCE_UART_TX, AF_UART);
	GPIO_PinAFConfig(PORT_UART, PINSOURCE_UART_RX, AF_UART);

	// Setup UART
	USART_InitTypeDef uartInit;
	uartInit.USART_BaudRate = 9600; //115200;
	uartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uartInit.USART_Parity = USART_Parity_No;
	uartInit.USART_StopBits = USART_StopBits_1;
	uartInit.USART_WordLength = USART_WordLength_8b;
	uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART4, &uartInit);

	USART_OverSampling8Cmd(UART4, DISABLE);
	USART_MSBFirstCmd(UART4, DISABLE);

	USART_Cmd(UART4, ENABLE);

	// Enable interrupt
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = UART4_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 2;
	nvicStructure.NVIC_IRQChannelSubPriority = 2;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

/**
 * uartWriteString: write string of bytes over UART
 *
 * Parameters:
 * 	s: sequence of characters
 */
void uartWriteString(char *s)
{
	while (*s)
	{
		uartWriteByte(*s++);
	}
}

/**
 * uartWriteByte: write single byte over UART
 *
 * Write a byte once the channel is available
 */
void uartWriteByte(uint8_t byte)
{
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
		; // Wait until empty
	USART_SendData(UART4, byte);
}

/**
 * uartWriteCommand: write command packet
 *
 * Each packet consists of 6 bytes:
 * 	[0] start byte
 * 	[1] command byte
 * 	[2] payload
 * 	[3,4] crc check
 * 	[5] stop byte
 */
void uartWriteCommand(uint8_t command, uint8_t payload)
{
	/* Build packet */
	uint8_t packet[UART_PACKET_SIZE];
	packet[0] = stx;
	packet[1] = command;
	packet[2] = payload;

	uint16_t crc = crc16(&packet[1], 2);

	packet[3] = crc >> 8;   // Higher bits
	packet[4] = crc & 0xFF; // Lower bits
	packet[5] = etx;

	/* Send packet */
	for (int i = 0; i < UART_PACKET_SIZE; i++)
		uartWriteByte(packet[i]);
}

/**
 * uartWriteCommandWithAck: write a command over UART and wait for an acknowledge
 *
 * Parameters:
 * 	command: command byte
 * 	payload: payload byte
 * 	delay: maximum iterations to wait for acknowledge
 */
void uartWriteCommandWithAck(uint8_t command, uint8_t payload, uint8_t delay)
{
	while (1)
	{
		uartWriteCommand(command, payload);

		/* Wait for ack/nack reception */
		uint8_t i = 0;
		while (!ackNackReceived && i < delay)
			i++;

		/* Continue only if ack received */
		if (ackNackReceived)
		{
			ackNackReceived = 0;
			if (ackValue)
			{
				return;
			}
		}
	}
}

/**
 * uartReadByte: Read byte from UART and write it to packet
 *
 * Handles start and stop flags: uartStartSend and uartStopSend
 * Constructs the packet of 6 bytes
 */
void uartReadByte()
{
	uint16_t ch = USART_ReceiveData(UART4); // Collect Char

	/* Manage start and end bytes to form packet */
	if (ch == stx)
	{
		uartStartSend = 1;
		uartIndex = 0;
	}
	else if (ch == etx)
		uartStopSend = 1;
	else if (uartIndex > UART_PACKET_SIZE - 1)
	{
		uartStartSend = 0;
		uartStopSend = 0;
		uartIndex = 0;
	}

	uartPacket[uartIndex++] = ch;
}

/**
 * uartReadAckNack: receive a (not) acknowledged byte
 */
uint8_t uartReadAckNack()
{
	uint16_t ch = USART_ReceiveData(UART4); // Collect Char
	if (ch == ack || ch == nak)
	{
		ackNackReceived = 1;
		ackValue = (ch == ack);
		return 1;
	}
	return 0;
}

/**
 * crc16: calculate 16-bits crc check value
 */
uint16_t crc16(uint8_t packet[], uint8_t len)
{
	uint16_t crc = 0xFFFF;

	uint8_t i;
	for (i = 0; i < len; i++)
	{
		uint8_t x;
		x = crc >> 8 ^ packet[i];
		x ^= x >> 4;
		crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
	}

	return crc;
}

/**
 * isCrcValid: check whether the crc in the packet matches the calculated crc
 */
uint8_t isCrcValid()
{
	uint8_t packet[2];
	packet[0] = command;
	packet[1] = payload;

	uint16_t crc = (uartPacket[3] << 8) | uartPacket[4];

	return crc == crc16(packet, 2);
}

/**
 * ackCmdValid: check crc for valid command
 *
 * returns:
 * 	1: crc is valid, ack send back
 * 	0: crc is invalid, nack send back
 */
uint8_t ackCmdValid()
{
	if (!isCrcValid())
	{
		uartWriteByte(nak);
		return 0;
	}
	else
	{
		uartWriteByte(ack);
		return 1;
	}
}
