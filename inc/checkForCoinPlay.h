/*
 * checkForCoinPlay.h
 *
 *  Created on: 1 Sep 2017
 *      Author: chanselaar
 */

#ifndef CHECKFORCOINPLAY_H_
#define CHECKFORCOINPLAY_H_

#include "stm32f30x.h"

int coinInsertDetection(void);
int checkcoin(void);
int whatStackToFill(void);
int whatStackToEmpty(void);
int queryLightGate(void);
uint8_t resetBit(uint8_t byte, uint8_t bit);

#endif /* CHECKFORCOINPLAY_H_ */
