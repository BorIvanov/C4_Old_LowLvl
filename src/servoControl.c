#include "servoControl.h"
#include "vars.h"
#include "motorpwm.h"
#include "motorControlInterrupt.h"
#include "stdlib.h"

/* Servo's are controlled using pulse width modulation (PWM) */

/**
 * initServoPWMs: Initialize pins and timers to control servo at 50Hz
 */

void initServoPWMs(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;		   //structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; //structure for TIM Time Base
	TIM_OCInitTypeDef TIM_OCInitStructure;		   //structure for TIM Output Compare

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //enable AHB port A
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //enable the TIM1 clock

	TIM_CtrlPWMOutputs(TIM2, DISABLE); //disable the PWM output
	TIM_Cmd(TIM2, DISABLE);

	/* Pin configuration */
	// Pin A0 and  A1 (PWM)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_1);

	/* TIM configuration for 50Hz */
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_Period = 19999; // PWM Period length
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM2, DISABLE);
	TIM_InternalClockConfig(TIM2);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Disable);

	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	// Timer7 is not running yet, so use low level control.
	setHorizontalVerticalServo(GRIPPER_HOMING_ANGLE_SAFE);	// Set gripper to starting position
	SetHVServoTimer();
	CloseEmptyingServo();									// Set slider to starting position
}

void SetHVServoTimer()
{
	TIM_SetCompare1(TIM2, servoPosition);
	moveServo = 0;
}

/**
 * setHorizontalVerticalServo: Set value for rotational servo
 *
 * Parameters:
 * 	posDeg: rotational position in degrees
 *
 * 	returns -1 on error, 0 is ok
 */

int setHorizontalVerticalServo(uint32_t posDeg)
{
	/*
	 * Transform degrees into duty cycle
	 * Minimum: 500
	 * Maximum: 2300
	 * Range: ~180 degrees
	 */
	EnableMotors(0, 0, 1);
	int offsetBase = 500;
	int posGain = 10;

	servoPosition = (posDeg * posGain) + offsetBase;
	moveServo = 1;
	DisableMotors(0, 0, 1);
	return 0;
}

int SetEmptyServoMan(uint32_t distance)
{
	TIM_SetCompare2(TIM2, distance);
	return 0;
}

void CloseEmptyingServo(void)
{
	TIM_SetCompare2(TIM2, EMPTYSERVOCLOSED);
}

void setEmptyingServoPosition(int pos)
{
	TIM_SetCompare2(TIM2, empty[pos]);
}
