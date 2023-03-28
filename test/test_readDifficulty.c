/*
 * test_motorPIDs.c
 *
 *  Created on: 17 apr. 2018
 *      Author: jajansen
 */

//#include "../vendor/ceedling/vendor/unity/src/unity.h"
#include "motorPIDs.h"
#include "encoder.h"
//#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "motorpwm.h"
#include "homing.h"
#include "motorControlInterrupt.h"
#include "servoControl.h"
#include "system_stm32f30x.h"
#include "stm32f30x_misc.h"

#include "readDifficulty.h"

#include "mock_stm32f30x_gpio.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_readDifficulty_Setting1(void)
{
	GPIO_SetBits_Ignore();
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOC, GPIO_Pin_4, 0);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_5, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_6, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_7, 1);

	TEST_ASSERT_EQUAL_UINT8(0x00, readDifficulty());
}

void test_readDifficulty_Setting2(void)
{
	GPIO_SetBits_Ignore();
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOC, GPIO_Pin_4, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_5, 0);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_6, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_7, 1);

	TEST_ASSERT_EQUAL_UINT8(0x01, readDifficulty());
}

void test_readDifficulty_Setting3(void)
{
	GPIO_SetBits_Ignore();
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOC, GPIO_Pin_4, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_5, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_6, 0);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_7, 1);

	TEST_ASSERT_EQUAL_UINT8(0x02, readDifficulty());
}

void test_readDifficulty_Setting4(void)
{
	GPIO_SetBits_Ignore();
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOC, GPIO_Pin_4, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_5, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_6, 1);
	GPIO_ReadInputDataBit_ExpectAndReturn(GPIOA, GPIO_Pin_7, 0);

	TEST_ASSERT_EQUAL_UINT8(0x02, readDifficulty());
}
