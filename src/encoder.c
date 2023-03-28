/**
  ******************************************************************************
  * @file    interupt.c
  * @author  Leroy Boerefijn
  * @version V1.0
  * @date    23-August-2017
  * @brief   Initializing interupt timer.
  ******************************************************************************
*/

#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"

// encoder intiation
/* Keep in mind that the order of inputs into the encoder is of crucial importance to direction sensing.
 * The 'logical' method is applicable here: The A-wire enters the lowest pin entry of each encoder, and
 * the B-wire enters the higher entry point.
 */
void initEncoder()
{
	GPIO_InitTypeDef GPIO_InitStructure; //structure for GPIO setup

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); //enable the AHB Peripheral Clock to use GPIOB

	/* MOTOR ENCODER 1 (X DIRECTION) */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //enable the TIM3 clock

	/* Pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_2);

	/* Configure the timer */
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,
							   TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/* TIM3 counter enable */
	TIM_Cmd(TIM3, ENABLE);

	/* add capability for interrupts*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* MOTOR ENCODER 2 (z DIRECTION) */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //enable the TIM4 clock

	/* Pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_2);

	/* Configure the timer */
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,
							   TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/* TIM4 counter enable */
	TIM_Cmd(TIM4, ENABLE);

	/* add capability for interrupts*/
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void initTimer3Interrupt()
{
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 14;
	nvicStructure.NVIC_IRQChannelSubPriority = 14;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

void initTimer4Interrupt()
{
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 13;
	nvicStructure.NVIC_IRQChannelSubPriority = 13;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

/**
 * getEncoderXPosition: retrieve x-position data of encoder
 *
 * Returns:
 * Encoder position in number of flanks
 */
uint32_t getEncoderXPosition()
{
	return TIM3->CNT;
}

/**
 * getEncoderZPosition: retrieve z-position data of encoder
 *
 * Returns:
 * Encoder position in number of flanks
 */
uint16_t getEncoderZPosition()
{
	return TIM4->CNT;
}
