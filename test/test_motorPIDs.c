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
#include "stm32f30x_misc.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_clampValue_Given100_ShouldReturn100(void)
{
	int min = -150;
	int max = 150;
	int input = 100;
	TEST_ASSERT_EQUAL_INT32(input, clampValue(input,min,max));
}

void test_clampValue_Given150_ShouldReturn150(void)
{
	int min = -150;
	int max = 150;
	int input = 150;
	TEST_ASSERT_EQUAL_INT32(input, clampValue(input,min,max));
}

void test_clampValue_GivenMin150_ShouldReturnMin150(void)
{
	int min = -150;
	int max = 150;
	int input = -150;
	TEST_ASSERT_EQUAL_INT32(input, clampValue(input,min,max));
}

void test_clampValue_Given200_ShouldReturn150(void)
{
	int min = -150;
	int max = 150;
	int input = 200;
	TEST_ASSERT_EQUAL_INT32(max, clampValue(input,min,max));
}

void test_clampValue_GivenMin200_ShouldReturnMin150(void)
{
	int min = -150;
	int max = 150;
	int input = -200;
	TEST_ASSERT_EQUAL_INT32(min, clampValue(input,min,max));
}

void test_clampValue_Given25_ShouldReturn50(void)
{
	int min = 50;
	int max = 150;
	int input = 25;
	TEST_ASSERT_EQUAL_INT32(min, clampValue(input,min,max));
}

void test_clampValue_Given201_ShouldReturn150(void)
{
	int min = 50;
	int max = 150;
	int input = 201;
	TEST_ASSERT_EQUAL_INT32(max, clampValue(input,min,max));
}

void test_checkPosDelta_Given100_100_ShouldReturn0(void)
{
	TEST_ASSERT_EQUAL_INT32(0, checkPosDelta(100, 100));
}

void test_checkPosDelta_Given100_200_ShouldReturn100(void)
{
	TEST_ASSERT_EQUAL_INT32(100, checkPosDelta(100, 200));
}

void test_checkPosDelta_Given200_100_ShouldReturn100(void)
{
	TEST_ASSERT_EQUAL_INT32(100, checkPosDelta(200, 100));
}

void test_checkPosDelta_Given2147483647_Minus2147483647_ShouldReturn294967294(void)
{
	TEST_ASSERT_EQUAL_INT32(294967294, checkPosDelta(147483647, -147483647));
}

void test_unwrapEncDelta_GivenHalfUint_ShouldReturnHalfUint(void)
{
	TEST_ASSERT_EQUAL_INT32(Mem_HalfUint, unwrapEncDelta(Mem_HalfUint));
}

void test_unwrapEncDelta_GivenZero_ShouldReturnZero(void)
{
	TEST_ASSERT_EQUAL_INT32(0, unwrapEncDelta(0));
}

void test_unwrapEncDelta_GivenHalfUintPlusOne_ShouldReturnOne(void)
{
	TEST_ASSERT_EQUAL_INT32(-Mem_HalfUint-1, unwrapEncDelta(Mem_HalfUint+1));
}

void test_PID_01(void)
{
	double Kp = 5.5;
	double Kd = 6.6;
	double Ki = 7.7;
	double period = 0.1;
	int32_t piderr = 100;
	int32_t delta = 10;
	double intCount = 0;

	double result = (Kp*piderr) + (-(delta/period)*Kd) + ((intCount*period)*Ki); // = -110

	TEST_ASSERT_DOUBLE_WITHIN(0.01, result, PID(Kp, Kd, Ki, period, piderr, delta, &intCount));
}

void test_calcAccelerationMX_NotMoving_ShouldReturnMinMotorSpeed(void)
{
	mx_SpeedCounter = 0;
	mx_FullPos = 60000;
	mx_MaxMotorSpeed = 5000;
	mx_MinMotorSpeed = 100;
	mx_HalfPosTarget = 30000;
	mx_acceleration = 25;
	mx_ismoving = 0;

	calcAccelration();

	TEST_ASSERT_EQUAL_INT32(mx_MinMotorSpeed, mx_RampMotorSpeed);
}

void test_calcAccelerationMX_CCW_ShouldReturn100xAcceleration(void)
{
	mx_SpeedCounter = 0;
	mx_FullPos = 60000;
	mx_MaxMotorSpeed = 5000;
	mx_MinMotorSpeed = 100;
	mx_HalfPosTarget = 30000;
	mx_acceleration = 25;
	mx_ismoving = -1;

	// execute 100 times
	for(int i=0; i<100; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(100*mx_acceleration, mx_RampMotorSpeed);
}

void test_calcAccelerationMX_CW_ShouldReturn100xAcceleration(void)
{
	mx_SpeedCounter = 0;
	mx_FullPos = 0;
	mx_MaxMotorSpeed = 5000;
	mx_MinMotorSpeed = 100;
	mx_HalfPosTarget = 30000;
	mx_acceleration = 25;
	mx_ismoving = 1;

	// execute 100 times
	for(int i=0; i<100; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(100*mx_acceleration, mx_RampMotorSpeed);
}

void test_calcAccelerationMX_CW_10000Times_ShouldReturnMaxMotorSpeed(void)
{
	mx_SpeedCounter = 0;
	mx_FullPos = 0;
	mx_MaxMotorSpeed = 5000;
	mx_MinMotorSpeed = 100;
	mx_HalfPosTarget = 30000;
	mx_acceleration = 25;
	mx_ismoving = 1;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(mx_MaxMotorSpeed, mx_RampMotorSpeed);
}

void test_calcAccelerationMX_CW_fullprofile_ShouldReturnMinMotorSpeed(void)
{
	mx_SpeedCounter = 0;
	mx_PosTarget = 60000;
	mx_FullPos = 0;
	mx_MaxMotorSpeed = 5000;
	mx_MinMotorSpeed = 100;
	mx_HalfPosTarget = 30000;
	mx_acceleration = 25;
	mx_ismoving = 1;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	// check inbetween profile
	TEST_ASSERT_EQUAL_INT32(mx_MaxMotorSpeed, mx_RampMotorSpeed);

	mx_FullPos = 30001;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(mx_MinMotorSpeed, mx_RampMotorSpeed);
}

void test_calcAccelerationMZ_NotMoving_ShouldReturnMinMotorSpeed(void)
{
	mz_SpeedCounter = 0;
	mz_FullPos = 60000;
	mz_MaxMotorSpeed = 5000;
	mz_MinMotorSpeed = 100;
	mz_HalfPosTarget = 30000;
	mz_acceleration = 25;
	mz_ismoving = 0;

	calcAccelration();

	TEST_ASSERT_EQUAL_INT32(mz_MinMotorSpeed, mz_RampMotorSpeed);
}

void test_calcAccelerationMZ_CCW_ShouldReturn100xAcceleration(void)
{
	mz_SpeedCounter = 0;
	mz_FullPos = 60000;
	mz_MaxMotorSpeed = 5000;
	mz_MinMotorSpeed = 100;
	mz_HalfPosTarget = 30000;
	mz_acceleration = 25;
	mz_ismoving = -1;

	// execute 100 times
	for(int i=0; i<100; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(100*mz_acceleration, mz_RampMotorSpeed);
}

void test_calcAccelerationMZ_CW_ShouldReturn100xAcceleration(void)
{
	mz_SpeedCounter = 0;
	mz_FullPos = 0;
	mz_MaxMotorSpeed = 5000;
	mz_MinMotorSpeed = 100;
	mz_HalfPosTarget = 30000;
	mz_acceleration = 25;
	mz_ismoving = 1;

	// execute 100 times
	for(int i=0; i<100; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(100*mz_acceleration, mz_RampMotorSpeed);
}

void test_calcAccelerationMZ_CW_10000Times_ShouldReturnMaxMotorSpeed(void)
{
	mz_SpeedCounter = 0;
	mz_FullPos = 0;
	mz_MaxMotorSpeed = 5000;
	mz_MinMotorSpeed = 100;
	mz_HalfPosTarget = 30000;
	mz_acceleration = 25;
	mz_ismoving = 1;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(mz_MaxMotorSpeed, mz_RampMotorSpeed);
}

void test_calcAccelerationMZ_CW_fullprofile_ShouldReturnMinMotorSpeed(void)
{
	mz_SpeedCounter = 0;
	mz_PosTarget = 60000;
	mz_FullPos = 0;
	mz_MaxMotorSpeed = 5000;
	mz_MinMotorSpeed = 100;
	mz_HalfPosTarget = 30000;
	mz_acceleration = 25;
	mz_ismoving = 1;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	// check inbetween profile
	TEST_ASSERT_EQUAL_INT32(mz_MaxMotorSpeed, mz_RampMotorSpeed);

	mz_FullPos = 30001;

	// execute 10000 times
	for(int i=0; i<10000; i++)
	{
		calcAccelration();
	}

	TEST_ASSERT_EQUAL_INT32(mz_MinMotorSpeed, mz_RampMotorSpeed);
}

void test_calcHalfPosTargetGiven0_10000_Shouldreturn5000(void)
{
	TEST_ASSERT_EQUAL_INT32(5000, calcHalfPosTarget(0, 10000));
}

void test_calcHalfPosTargetGiven5000_10000_Shouldreturn7500(void)
{
	TEST_ASSERT_EQUAL_INT32(7500, calcHalfPosTarget(5000, 10000));
}

void test_calcHalfPosTargetGiven10000_0_Shouldreturn5000(void)
{
	TEST_ASSERT_EQUAL_INT32(5000, calcHalfPosTarget(10000, 0));
}

void test_calcHalfPosTargetGiven10000_5000_Shouldreturn7500(void)
{
	TEST_ASSERT_EQUAL_INT32(7500, calcHalfPosTarget(10000, 5000));
}

void test_calcHalfPosTargetGiven0_10000_Shouldreturn1(void)
{
	TEST_ASSERT_EQUAL_INT(1, calcDirection(0, 10000));
}

void test_calcHalfPosTargetGiven5000_10000_Shouldreturn1(void)
{
	TEST_ASSERT_EQUAL_INT(1, calcDirection(5000, 10000));
}

void test_calcHalfPosTargetGiven10000_0_ShouldreturnMinus1(void)
{
	TEST_ASSERT_EQUAL_INT(-1, calcDirection(10000, 0));
}

void test_calcHalfPosTargetGiven10000_5000_ShouldreturnMinus1(void)
{
	TEST_ASSERT_EQUAL_INT(-1, calcDirection(10000, 5000));
}
