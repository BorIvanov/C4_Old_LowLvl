/*
 * test_tunePIDs.h
 *
 *  Created on: 16 Aug 2019
 *      Author: ashri
 */

#ifndef TEST_TUNEPIDS_H_
#define TEST_TUNEPIDS_H_

// function definitions
//void test_tuneZPIDs(const char* direction);
void test_tuneZPIDs(const char* direction);
void test_zStairs(const char* direction);
void test_benchmark();
void test_benchZ();

//void test_MotorZ(void);
//void test_MoveToPos(int pos, const char *axis);
void test_moveToZPos(int16_t targetZ, uint8_t fixedInterfal);
void test_moveToZPosNoWait(int16_t targetZ);
void test_moveToZPosTune(int16_t targetZ);
void test_moveToPosXY(int16_t targetX, int16_t targetZ);


// STM->RPI uart stuff
void uart_printf(char *format, ...);
uint8_t enableWriteString; // enable or disable uart_printf output


#endif /* TEST_TUNEPIDS_H_ */
