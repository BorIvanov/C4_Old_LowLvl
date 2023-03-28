/*
 * GPIO_init_functions.c
 *
 *  Created on: 30 Aug 2017
 *      Author: chanselaar
 *      Goal: 	initialise the various output GPIOs
 *      note: 	try to create seperate functions for each functionality, to allow better code readability.
 */

// includes
#include "GPIO_init_functions.h"
#include "stm32f30x.h"
#include "vars.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_syscfg.h"
#include "homing.h"

/**
 * initGPIOLEDs: initialize the LEDS
 */
void initGPIOLEDs()
{
	GPIO_InitTypeDef gpio;
	/* STM32F3Discovery */

	/* Custom STM */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Pin = DLEDS;
	GPIO_Init(DLED_PORT, &gpio);
}

/**
 * initRotarySwitch: Initialize the inputs for the rotary switch
 *
 * 4 Inputs are possible
 */
void initRotarySwitch()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_UP; // Pull up resistance
	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpio);

	for (int i = 0; i < 100000; i++)
		; // waste some time

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_UP; // Pull up resistance
	gpio.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &gpio);
}

/**
 * initHomeSwitches: Initialize the inputs for the home switches:
 * X-axis and Z-axis
 *
 */

void initHomeSwitches()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN; // Pull down resistance
	gpio.GPIO_Pin = HOMING_X | HOMING_Z;
	GPIO_Init(HOME_END_PORT, &gpio);
}

/**
 * initEStop: Initialize the input for reading out the Emergency Stop button
 *
 */
void initEStop()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN; // Pull down resistance
	gpio.GPIO_Pin = E_STOP_PIN;
	GPIO_Init(E_STOP_PORT, &gpio);
}

/**
 * initResetButton: Initialize the input for reading out the game reset button
 *
 */
void initResetButton()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_UP; // Pull down resistance
	gpio.GPIO_Pin = RST_BUTTON_PIN;
	GPIO_Init(RST_BUTTON_PORT, &gpio);
}

/**
 * initEndSwitches: Initialize the inputs for the end switches:
 * X-axis and Z-axis
 *
 *	Not yet physically implemented. If someone ever does or wants to, this code has not been tested or evaluated.
 *	These gpio pins are in relation with the emergency brake in the original set up.
 *
 */
void initEndSwitches()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN; // Pull down resistance
	gpio.GPIO_Pin = END_X | END_Z;
	GPIO_Init(HOME_END_PORT, &gpio);

	//	/* CONFIGURE AS INTERRUPTS: */
	//
	//	/* Set variables used */
	//	GPIO_InitTypeDef GPIO_InitStruct;
	//	EXTI_InitTypeDef EXTI_InitStruct;
	//	NVIC_InitTypeDef NVIC_InitStruct;
	//
	//	/*	----- X-Axis -----	*/
	//
	//	/* Enable clock for GPIOB */
	//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	//	/* Enable clock for SYSCFG */
	//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//
	//	/* configure pin as input */
	//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStruct.GPIO_Pin = END_X;
	//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//
	//	/* Tell system to use PB0 */
	//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	//
	//	/* PB0 is connected to EXTI_Line0 */
	//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	//	EXTI_Init(&EXTI_InitStruct);
	//
	//	/* Add IRQ vector to NVIC */
	//	/* PB0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStruct);
	//
	//	/*	----- Z-Axis -----	*/
	//
	//	/* Enable clock for GPIOB */
	//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	//	/* Enable clock for SYSCFG */
	//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//
	//	/* configure pin as input */
	//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	//	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//	GPIO_InitStruct.GPIO_Pin = END_Z;
	//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//
	//	/* Tell system to use PB1 */
	//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
	//
	//	/* PB1 is connected to EXTI_Line1 */
	//	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	//	EXTI_Init(&EXTI_InitStruct);
	//
	//	/* Add IRQ vector to NVIC */
	//	/* PB1 is connected to EXTI_Line1, which has EXTI1_IRQn vector */
	//	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStruct);
	//
	//	/* Waste some time in order for the input to stabilize after startup */
	//	for (int i=0; i<100000; i++);	//TODO: Check if this delay is needed when pulldown resistance from homeswitch board is used
	//
	//	/* Make sure the switches are connected and not triggered */
	//	//while(!GPIO_ReadInputDataBit(GPIOB, END_X) && !GPIO_ReadInputDataBit(GPIOB, END_Z));
}
