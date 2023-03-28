/*
 * vacControl.c
 *
 *  Created on: 4 Sep 2017
 *      Author: chanselaar
 *      goal: 	measurement of vacuum, and control of pump
 */

#include "stm32f30x.h"
#include "motorControlInterrupt.h"
#include "vars.h"

/*
 * initVacSens: Initializes the vacuum sensor analog input
 *
 */
void initVacSens()
{

	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure the ADC clock */
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div2);

	/* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

	/* ADC Channel configuration */
	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	/* Configure ADC Channel7 as analog input */
	GPIO_InitStructure.GPIO_Pin = VAC_SENSE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_StructInit(&ADC_InitStructure);

	/* Calibration procedure */

	ADC_VoltageRegulatorCmd(ADC2, ENABLE);
	/* Insert delay of at least 10 �s
	 *	wasteful delay, but custom wait function cannot be used
	 *	can only be used after timer interrupt is initialized
	 *	when this timer is initialized the sensor readout is already enabled
	 */
	for (int i = 0; i < 100000; i++)
		;

	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC2);

	while (ADC_GetCalibrationStatus(ADC2) != RESET)
		;

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;
	ADC_CommonInit(ADC2, &ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC1 regular channel7 configuration */
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 1, ADC_SampleTime_7Cycles5);

	/* Enable ADC1 */
	ADC_Cmd(ADC2, ENABLE);

	/* wait for ADRDY */
	while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY))
		;

	/* Start ADC1 Software Conversion */
	ADC_StartConversion(ADC2);
}

/**
 * initPump: initialize the vacuum pump GPIO
 *
 */
void initPump()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure port A, pin 3 as output */
	GPIO_InitStructure.GPIO_Pin = VAC_PUMP_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(VAC_PUMP_PORT, &GPIO_InitStructure);

	GPIO_SetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);   // Disable valve after init
	GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN); // Disable valve after init
}

/**
 * initValve: initialize the vacuum valve GPIO
 *
 */
void initValve()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	/* Configure port C, pin 0 as output */
	GPIO_InitStructure.GPIO_Pin = VAC_VALVE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(VAC_VALVE_PORT, &GPIO_InitStructure);

	GPIO_SetBits(VAC_VALVE_PORT, VAC_VALVE_PIN); // Disable valve after init
}

/**
 * vacPressMeasure: measure vacuum pressure
 *	This function returns the vacuum pressure value in bar
 *
 *	NOTE: Minimum possible vacuum measurement is 0.84 bar
 */
double vacPressMeasure()
{
	// measuring pressure
	// 1. query the sensor
	// 2. transform voltage to pressure [bar]
	// 3. output the value
	__IO uint16_t ADC2ConvertedValue = 0;

	/* Test EOC flag */
	while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
		;

	/* Get ADC1 converted data */
	ADC2ConvertedValue = ADC_GetConversionValue(ADC2);

	/* Compute the voltage (3.3V @ 12bit resolution) and convert to bar*/
	//return ((ADC2ConvertedValue / (float)(5/3.3*4095) ) + 0.095 ) / (float)0.9;

	return ADC2ConvertedValue;
}

/**
 * vacControl: control vacuum pump and valve
 *
 * Pressure is controlled by turning the pump on or off.
 * A hysteresis loop is specified with a max in min value
 *
 * Parameters:
 * 	pMeasure: current pressure
 * 	pDeltaMin: minimal pressure
 * 	pDeltaMax: maximal pressure
 *
 * Returns:
 * 	0: Pressure established OK
 * 	-1: error
 * TODO should it really return an error code?
 */
int vacControl(double pMeasure, double pDeltaMin, double pDeltaMax)
{
	// calculate where we are in the hysteresis loop and turn pump on and off

	if (pMeasure > pDeltaMin) // if below lower delta limit, turn on
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	}
	if (pMeasure < pDeltaMax) // if charged to upper delta limit, turn off
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}

	return 0; // 0 is ok, -1 is error
}
