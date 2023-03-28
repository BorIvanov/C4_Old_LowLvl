/*
 * test_uart.c
 *
 *  Created on: 23 apr. 2018
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
#include "uart.h"
#include "vars.h"
#include "stm32f30x_usart.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_CRC16_IN7_OUT0xD641(void)
{
	uint8_t packet[2];
	packet[0] = 0x07;
	packet[1] = 0x07;

	TEST_ASSERT_EQUAL_UINT16(62591, crc16(packet, 2));
}
