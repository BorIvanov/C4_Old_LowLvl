/*
 * test_homing.h
 *
 *  Created on: 16 Aug 2019
 *      Author: ashri
 */

#ifndef TEST_HOMING_H_
#define TEST_HOMING_H_

/*
 * function declaration
 */
void test_endSwitchesInput(void);
uint8_t test_xSwitch(void);
void test_moveXMotor(int16_t xDuty);
void test_homing(int16_t xDuty);
void test_movement();

#endif /* TEST_HOMING_H_ */
