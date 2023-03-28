/*
 * test_motorPIDs.c
 *
 *  Created on: 17 apr. 2018
 *      Author: jajansen
 */

//#include "../vendor/ceedling/vendor/unity/src/unity.h"
#include "motorPIDs.h"
#include "encoder.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "motorpwm.h"
#include "homing.h"
#include "motorControlInterrupt.h"
#include "servoControl.h"
#include "system_stm32f30x.h"
#include "mock_i2c.h"
#include "stm32f30x_misc.h"
#include "rgb_light_control.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_getHue_Given65535_65535_65535_ShouldReturn0(void)
{
	TEST_ASSERT_EQUAL_INT32(0, getHue(65535, 65535, 65535));
}

void test_getHue_Given0_0_0_ShouldReturn0(void)
{
	TEST_ASSERT_EQUAL_INT32(0, getHue(0, 0, 0));
}

void test_getHue_Given65535_0_0_ShouldReturn0(void)
{
	TEST_ASSERT_EQUAL_INT32(0, getHue(65535, 0, 0));
}

void test_getHue_Given0_65535_0_ShouldReturn120(void)
{
	TEST_ASSERT_EQUAL_INT32(120, getHue(0, 65535, 0));
}

void test_getHue_Given0_0_65535_ShouldReturn240(void)
{
	TEST_ASSERT_EQUAL_INT32(240, getHue(0, 0, 65535));
}

void test_getHue_Given64880_16713_655_ShouldReturn15(void)
{
	TEST_ASSERT_EQUAL_INT32(15, getHue(64880, 16713, 655));
}

void test_getHue_Given64880_655_52035_ShouldReturn312(void)
{
	TEST_ASSERT_EQUAL_INT32(312, getHue(64880, 655, 52035));
}

void test_getHue_Given655_64880_64880_ShouldReturn180(void)
{
	TEST_ASSERT_EQUAL_INT32(180, getHue(655, 64880, 64880));
}
