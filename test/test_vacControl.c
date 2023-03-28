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

#include "vacControl.h"

#include "mock_stm32f30x_gpio.h"
#include "mock_stm32f30x_adc.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_vacPressMeasure_BitValueZero_ShouldReturnZero(void)
{
	ADC_GetFlagStatus_ExpectAndReturn(ADC2, ADC_FLAG_EOC, SET );

	ADC_GetConversionValue_ExpectAndReturn(ADC2, 0);

	TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.1, vacPressMeasure());
}

void test_vacPressMeasure_BitValue4095_ShouldReturn0_84(void)
{
	ADC_GetFlagStatus_ExpectAndReturn(ADC2, ADC_FLAG_EOC, SET );

	ADC_GetConversionValue_ExpectAndReturn(ADC2, 4095);

	TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.84, vacPressMeasure());
}

void test_vacPressMeasure_BitValue3447_ShouldReturn0_72(void)
{
	ADC_GetFlagStatus_ExpectAndReturn(ADC2, ADC_FLAG_EOC, SET );

	ADC_GetConversionValue_ExpectAndReturn(ADC2, 3477);

	TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.72, vacPressMeasure());
}

void test_vacControlOn(void)
{
	GPIO_SetBits_Expect(GPIOA, GPIO_Pin_3);

	vacControl(0.8, 0.6, 0.8);
}

void test_vacControlOff(void)
{
	GPIO_ResetBits_Expect(GPIOA, GPIO_Pin_3);

	vacControl(0.5, 0.6, 0.8);
}

void test_vacControlInbetween(void)
{
	//TODO: write test

}


