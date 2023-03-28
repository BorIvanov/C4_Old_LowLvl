/*
 * readDifficulty.c
 *
 *  Created on: 12 Sep 2017
 *      Author: chanselaar
 */

#include "readDifficulty.h"

#define LOW 0x00
#define MEDIUM 0x01
#define HIGH 0x02

/**
 * readDifficulty: read the set rotary switch value
 *
 * Returns:
 * 	0x00: low difficulty
 * 	0x01: medium difficulty
 * 	0x02: maximum difficulty
 *
 * Note: in current implementation, both 3th and 4th setting are maximum difficulty
 */
unsigned char readDifficulty(void)
{
	unsigned char difficulty = 0x00;
	if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4))
	{
		difficulty = LOW;
	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5))
	{
		difficulty = MEDIUM;
	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
	{
		difficulty = HIGH;
	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7))
	{
		difficulty = HIGH;
	}
	return difficulty;
}
