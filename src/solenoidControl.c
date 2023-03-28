/*
 * solenoidControl.c
 *
 *  Created on: 23 April 2018
 *      Author: jajansen
 */

#include "solenoidcontrol.h"
#include "stm32f30x.h"

/**
 * initSolenoid: Initialize the output to control the 'flipper' solenoid
 *
 */
void initSolenoid()
{
	/* DO NOT DELETE: WORKING CODE WITHOUT PWM!
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit( &gpio );
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &gpio);
	*/

	/* TESTING CODE FOR PWM CONTROL */
	GPIO_InitTypeDef GPIO_InitStructure;		   // structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // structure for TIM Time Base
	TIM_OCInitTypeDef TIM_OCInitStructure;		   // structure for TIM Output Compare

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);   // enable AHB port A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE); // enable the TIM15 clock

	/* Pin configuration */
	// Pin A2  (PWM)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_9); // AF9 = TIM15@PA2, see datasheet

	/* TIM configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 71; // System clock = 72 MHz. Timer runs at 1MHz (72-1)
	TIM_TimeBaseStructure.TIM_Period = 4999;  // PWM@200 Hz | 1MHz/(4999+1)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM15, DISABLE);
	TIM_InternalClockConfig(TIM15);
	TIM_OC1PreloadConfig(TIM15, TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM15, &TIM_OCInitStructure);

	TIM_SelectMasterSlaveMode(TIM15, TIM_MasterSlaveMode_Disable);

	// Enable the PWM output
	TIM_CtrlPWMOutputs(TIM15, ENABLE);
	TIM_Cmd(TIM15, ENABLE);

	// Set duty cycle at 0
	TIM_SetCompare1(TIM15, 0);
}

/**
 * controlSolenoid: Use flipper to push chip towards human
 *
 * First enables the solenoid to push the chip.
 * Short delay
 * Disable the solenoid again.
 */
void controlSolenoid(void)
{
	/*setSolenoidDutyCycle(70);
	waitFunction(30);
	//setSolenoidDutyCycle(0);
	TIM_SetCompare1(TIM15,0);
	waitFunction(15);*/
	setSolenoidDutyCycle(88);
	waitFunction(20);
	setSolenoidDutyCycle(0);
	//TIM_SetCompare1(TIM15,0);
	//waitFunction(50);
}

/**
 * setSolenoidDutyCycle: Sets the PMW output of solenoid to a value between 0 and 100%
 * this value controls the force of the actuator
 * input limited between 0 (off) and 100 (fully on)
 */

void setSolenoidDutyCycle(uint32_t dutycycle)
{
	/*clamp between 0 and 100*/
	if (dutycycle < 0)
		dutycycle = 0;
	if (dutycycle > 100)
		dutycycle = 100;

	// set output compare register to value between 0 and 5000
	TIM_SetCompare1(TIM15, dutycycle * 50);
}
