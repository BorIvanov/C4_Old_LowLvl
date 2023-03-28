/*
 * TimerSettings.c
 *
 *  Created on: 30 Aug 2017
 *      Author: chanselaar
 *      Goal: 	1. Ensure that the timer 7 is correctly initialized.
 *      		2. Ensure that the interrupts are enabled if applicable
 *      		3. write the execute functions that will be calle during the respective interrupts.
 *  Changed on: 15 Mar 2018
 *  	Author:	Jasper Jansen
 *  	Descr.:	Timer was initialized with
 *  				Prescaler: 	36000
 *  				Period:		20
 *
 *  			This results in a frequency of ~95.2Hz (72Khz / ((36000+1)*(20+1)))
 *
 *  			Timer init changed to:
 *  				Prescaler:	35999
 *  				Period:		19
 *
 *  			Resulting in a frequency of 100Hz (72Khz / ((35999+1)*(19+1)))
 */
#include "stm32f30x.h"
#include "vars.h"
#include "motorControlInterrupt.h"

/**
 * initTimer7: Initialize timer 7
 * Runs at 100 Hz (10ms interval)
 */
void initTimer7()
{
	/* start up the capability of the appropriate timer */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	/* define the structure, main frequency at 72MHz */
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 35999;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 19;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;

	/* write the timerInitStructure to TIM7 */
	TIM_TimeBaseInit(TIM7, &timerInitStructure);

	/* activate TIM7 */
	TIM_Cmd(TIM7, ENABLE);
}

/**
 * initTimer7Interrupt: Enable the interrupt for the timer
 */
void initTimer7Interrupt()
{
	// add capability for interrupts
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	// initiate interrupt settings
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM7_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 15;
	nvicStructure.NVIC_IRQChannelSubPriority = 15;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

/**
 * waitFunction: Wait an integer number of 10 ms time steps.
 *
 * So a waitTime of 100 pauses the program for 990 ms plus 0 to 10 ms.
 * WARNING: a wait step of 1 is not defined properly, as you may
 * reset the counter right before an interrupt, effectively waiting
 * less than 10 ms.
 *
 * Parameters:
 * 	waitTime: integer number of 10 ms steps to wait
 */
void waitFunction(uint16_t waitTime)
{
	TIM7_CustomTimerCounter = 0;
	while (TIM7_CustomTimerCounter < waitTime)
		;
}
