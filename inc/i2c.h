/*
 * i2c.h
 *
 *  Created on: 4 Sep 2017
 *      Author: lboerefijn
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f30x.h"

#define PIN_I2C_SCL GPIO_Pin_8
#define PIN_I2C_SDA GPIO_Pin_9
#define PINSOURCE_I2C_SCL GPIO_PinSource8
#define PINSOURCE_I2C_SDA GPIO_PinSource9
#define PORT_I2C GPIOB
#define AF_I2C GPIO_AF_4
#define I2C_ADD_RGB 0x29 << 1
#define I2C_ADD_PROX 0x13 << 1
#define I2C_ADD_CD 0x38 << 1
#define RGB_COMMAND_REG 0x80

unsigned char buffer[5];

void initI2C();
void initCD();
void sendData(uint8_t address, uint8_t nBytes);
void receiveData(uint8_t address, uint8_t nBytes);
void sendReceiveData(uint8_t address, uint8_t nBytesSend, uint8_t nBytesReceive);

#endif /* I2C_H_ */
