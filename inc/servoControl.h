/*
 * servo.h
 *
 *  Created on: 7 Sep 2017
 *      Author: lboerefijn
 */

#ifndef SERVOCONTROL_H_
#define SERVOCONTROL_H_

#include "stm32f30x.h"
int SetEmptyServoMan(uint32_t distance);
void SetHVServoTimer();
int setHorizontalVerticalServo(uint32_t posDeg);
void initServoPWMs(void);
void EmptyBoard(uint8_t stage);
void CloseEmptyingServo(void);
void setEmptyingServoPosition(int pos);

#endif /* SERVOCONTROL_H_ */
