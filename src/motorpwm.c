/**
 ******************************************************************************
 * @file    motorpwm.c
 * @author  Leroy Boerefijn
 * @version V1.0
 * @date    22-August-2017
 * @brief   Initializing PWM and setting duty cycle.
 ******************************************************************************
 */

#include "stm32f30x.h"
#include "vars.h"
#include "homing.h"
#include "motorPIDs.h"
#include "stdlib.h"
#include "motorpwm.h"

/**
 * motorPWMInit: initialize motor PWM control
 *
 * Initialises pins for PWM output, direction, and power enable
 * Configures the timer for PWM generation
 */
void initMotorPWM()
{
	DisableMotors(1, 1, 1);						   // Disable motor power for X and Z
	GPIO_InitTypeDef GPIO_InitStructure;		   //structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; //structure for TIM Time Base
	TIM_OCInitTypeDef TIM_OCInitStructure;		   //structure for TIM Output Compare

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //enable AHB port A
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);  //enable AHB port C
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //enable the TIM1 clock

	/* Pin configuration */
	// Pin A8 and A9 (PWM)
	GPIO_InitStructure.GPIO_Pin = MX_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MX_PWM_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(MX_PWM_PORT, MX_PWM_PINSOURCE, MX_PWM_AF);

	GPIO_InitStructure.GPIO_Pin = MZ_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MZ_PWM_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(MZ_PWM_PORT, MZ_PWM_PINSOURCE, MZ_PWM_AF);

	// Pin C9 (direction)
	GPIO_InitStructure.GPIO_Pin = MX_DIR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MX_DIR_PORT, &GPIO_InitStructure);
	// Pin c8 (motor 2 direction)
	GPIO_InitStructure.GPIO_Pin = MZ_DIR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MZ_DIR_PORT, &GPIO_InitStructure);

	// Pin C2 motor power enable
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MX_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MX_EN_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MZ_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(MZ_EN_PORT, &GPIO_InitStructure);

	/* TIM configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 9;
	TIM_TimeBaseStructure.TIM_Period = 1999; // PWM Period length
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_InternalClockConfig(TIM1);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Disable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE); // Enable the PWM output
	TIM_Cmd(TIM1, ENABLE);

	// Set duty cycle at 0
	TIM_SetCompare1(TIM1, 0);
	TIM_SetCompare2(TIM1, 0);

	// Set direction output at 0
	GPIO_SetBits(MX_DIR_PORT, MX_DIR_PIN);
	GPIO_SetBits(MZ_DIR_PORT, MZ_DIR_PIN);

	MotorPWMduty(0, 0);
}

/**
 * MotorPWM set dutycycle: sets the duty cycle for both motors
 *
 * parameter
 * motx: the pwm value for motorx, + is away from z spindle, - is towards z spindle
 * motz: the pwm value for motorz, + is upwards, - is downwards.
 * also see cad files
 */
void MotorPWMduty(double motx, double motz)
{
	motx = clampValue(motx, -100, 100);
	motz = clampValue(motz, -100, 100);

	motx = motx / 3;

	int pwmx;
	int pwmz;
	//input abs 0 to 100%
	//output between 10 and 90%. therefore 0.8 used of total range. and then scaling +10%
	pwmx = abs(motx) * (0.8 * 2048) / 100 + (0.1 * 2047);
	pwmz = abs(motz) * (0.8 * 2048) / 100 + (0.1 * 2048);
	//pwmx = pwmx/2;
	/* Set direction pin based on sign of pwmDem*/
	if (motx < 0)
	{
		GPIO_SetBits(MX_DIR_PORT, MX_DIR_PIN);
	}
	else
	{
		GPIO_ResetBits(MX_DIR_PORT, MX_DIR_PIN);
	}

	if (motz < 0)
	{
		GPIO_SetBits(MZ_DIR_PORT, MZ_DIR_PIN);
	}
	else
	{
		GPIO_ResetBits(MZ_DIR_PORT, MZ_DIR_PIN);
	}

	TIM_SetCompare2(TIM1, pwmx);
	TIM_SetCompare1(TIM1, pwmz);
}

/**
 * motorPWMSetDutyCycle: set duty cycles for both motors
 *
 * Parameters:
 * 	motorX: duty cycle for motor x; value between 0 and 1000
 * 	motorZ: duty cycle for motor z; value between 0 and 1000
 */
void motorPWMSetDutyCycle(int motorX, int motorZ)
{
	/* controllerPWm runs from 0-4095
	 * 10% -> 41
	 * 90% -> 3686
	 */

	/* Clip value between -1000 and 1000 */
	motorX = clampValue(motorX, -100, 100);
	motorZ = clampValue(motorZ, -1000, 1000);

	/* PWM output always between 10% and 90%
	 * That translates to values between 200 and 1800 */
	uint32_t outMX = abs((int)(2 * (double)motorX * 0.75));
	uint32_t outMZ = abs((int)(2 * (double)motorZ * 0.75));

	/* Set direction pin based on sign of pwmDem*/
	if (motorX < 0)
		GPIO_SetBits(MX_DIR_PORT, MX_DIR_PIN);

	else
		GPIO_ResetBits(MX_DIR_PORT, MX_DIR_PIN);

	/* Set direction pin based on sign of pwmDem*/
	if (motorZ < 0)
	{
		GPIO_SetBits(MZ_DIR_PORT, MZ_DIR_PIN);
	}

	else
	{
		GPIO_ResetBits(MZ_DIR_PORT, MZ_DIR_PIN);
	}

	outMX += 201;
	outMZ += 201;

	TIM_SetCompare2(TIM1, outMX);
	TIM_SetCompare1(TIM1, outMZ);
}

/**
 * EnableMotors: Procedure for enabling the motor power
 *
 */
void EnableMotors(int x, int z, int servo)
{
	//MotorPWMduty(0,0);
	if (x==1)	{EnableXMotors();}
	if (z==1)	{EnableZMotors();}
	if (servo==1)	{EnableServoMotors();}

}

void EnableXMotors()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2);	// motor power enable
	GPIO_SetBits(GPIOA, GPIO_Pin_11);    // X axis enable
	//GPIO_SetBits(GPIOC, GPIO_Pin_7); 	// Z axis enable
	//TIM_CtrlPWMOutputs(TIM2, ENABLE);    // enable the servo PWM output
}

void EnableZMotors()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2);	// motor power enable
	//GPIO_SetBits(GPIOA, GPIO_Pin_11);    // X axis enable
	GPIO_SetBits(GPIOC, GPIO_Pin_7); 	// Z axis enable
	//TIM_CtrlPWMOutputs(TIM2, ENABLE);    // enable the servo PWM output
}

void EnableServoMotors()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2);	// motor power enable
	//GPIO_SetBits(GPIOA, GPIO_Pin_11);    // X axis enable
	//GPIO_SetBits(GPIOC, GPIO_Pin_7); 	// Z axis enable
	TIM_CtrlPWMOutputs(TIM2, ENABLE);    // enable the servo PWM output
}
/**
 * DisableMotors: Procedure for disabling the motor power
 *
 */
void DisableMotors(int x, int z, int servo)
{
	if (x==1)	{DisableXMotors();}
	if (z==1)	{DisableZMotors();}
	if (servo==1)	{DisableServoMotors();}

}

void DisableXMotors()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void DisableZMotors()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}

void DisableServoMotors()
{
	TIM_CtrlPWMOutputs(TIM2, DISABLE);
}
