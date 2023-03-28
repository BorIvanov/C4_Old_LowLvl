/*
 * gamePlay.h
 *
 *  Created on: 25 Sep 2017
 *      Author: lboerefijn
 */

#ifndef GAMEPLAY_H_
#define GAMEPLAY_H_

// Defines
#define CMD_INIT 0x31
#define CMD_START 0x32
#define CMD_DIFFICULTY 0x33
#define CMD_COLUMN 0x34
#define CMD_INSERT_COIN 0x35
#define CMD_GAME_END 0x36
#define CMD_NO_PAYLOAD 0x99
#define CMD_BOOTUP 0x61
#define CMD_SHUTDOWN 0x63
#define CMD_CHEATER 0x67

#define STATE_INIT 0x31
#define STATE_START 0x32
#define STATE_DIFFICULTY 0x33
#define STATE_COLUMN 0x34
#define STATE_INSERT_COIN 0x35
#define STATE_GAME_END 0x36
#define STATE_NO_PAYLOAD 0x99
#define STATE_BOOTUP 0x61
#define STATE_SHUTDOWN 0x63
#define STATE_CHEATER 0x67

void initGamePlay();
void startGamePlay();
void selectDifficulty();
void detectHumanInput();
void insertCoin();
void gameEnd();
int standardPlayLoop(uint8_t columnNr, int prevValue);
void performEndOfGameLogic();
uint8_t humanSelectedColumnDemo(uint8_t columnNr);

#endif /* GAMEPLAY_H_ */
