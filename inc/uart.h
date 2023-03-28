/*
 * uart.h
 *
 *  Created on: 4 Sep 2017
 *      Author: lboerefijn
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>

#include "stm32f30x.h"

#define PIN_UART_TX GPIO_Pin_10
#define PIN_UART_RX GPIO_Pin_11
#define PINSOURCE_UART_TX GPIO_PinSource10
#define PINSOURCE_UART_RX GPIO_PinSource11
#define PORT_UART GPIOC
#define AF_UART GPIO_AF_5
#define BYTE_STX 0x06
#define BYTE_ETX 0x09

#define stx 0x96
#define etx 0xC8
#define ack 0xEF
#define nak 0xEA

#define UART_PACKET_SIZE 6

uint8_t uartPacket[UART_PACKET_SIZE];
uint8_t uartStartSend;
uint8_t uartStopSend;
uint8_t uartIndex;
uint8_t command;
uint8_t payload;
uint8_t ackNackReceived;
uint8_t ackValue;

void initUART();
void uartWriteString(char *s);
void uartWriteByte(uint8_t byte);
void uartWriteCommand(uint8_t command, uint8_t payload);
void uartWriteCommandWithAck(uint8_t command, uint8_t payload, uint8_t delay);
void uartReadByte();
uint8_t uartReadAckNack();
uint16_t crc16(uint8_t packet[], uint8_t len);
uint8_t isCrcValid();
uint8_t ackCmdValid();

#endif /* UART_H_ */
