/*
 * test_homing.c
 *
 *  Created on: 19 Mar 2019
 *      Author: ashri
 */

#include "vars.h"
#include "test_homing.h"



/*
 * test entry point
 */
void test_HomingMain(void)
{
	uint8_t test_case = 3;

	switch(test_case)
	{
		case 0:
		{
			test_endSwitchesInput();
			break;
		}
		case 1:
		{
			test_xSwitch();
			break;
		}
		case 2:
		{
			test_moveXMotor(-1);
			break;
		}
		case 3:
		{
			test_homing(30);
			test_movement();
			break;
		}
	}// end test_case

	while(1)
	{
		// infinite loop after completing tasks
	}
}


/* test_EndSwitches
 *
 * Procedure used for testing the different channels of end switches
 */
void test_endSwitchesInput(void)
{
	int endx=0;
	int endz=0;
	int stopx=0;
	int stopz=0;

	uart_printf("Start EndSwitchesInput test\n");
	while (1)
	{
		endx = GPIO_ReadInputDataBit(HOME_END_PORT, END_X);
		endz = GPIO_ReadInputDataBit(E_STOP_PORT, E_STOP_PIN);
		stopx = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X);
		stopz = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z);
		uart_printf("EX: 0x%x, EZ: 0x%x, HX: 0x%x, HZ: 0x%x\n", endx,endz,stopx,stopz);
/*	// continuously read all switches
		if (GPIO_ReadInputDataBit(E_STOP_PORT, E_STOP_PIN))
		{
			uart_printf("END SWITCH X ACTIVATED\n");
		}
		if (GPIO_ReadInputDataBit(HOME_END_PORT, END_Z))
		{
			uart_printf("END SWITCH Z ACTIVATED\n");
		}
		if (GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X))
		{
			uart_printf("HOME SWITCH X ACTIVATED\n");
		}
		if (GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z))
		{
			uart_printf("HOME SWITCH Z ACTIVATED\n");
		}*/
		waitFunction(50); // 50ms between measurements
	}
}


/*
 * checks if switch is pressed
 */
uint8_t test_xSwitch(void)
{
	uint8_t btnState = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X);
	uart_printf("x-switch: %d", btnState);
	return btnState;
}


/*
 * moves the motor in open loop
 * xDuty => range 0-1000
 */
void test_moveXMotor(int16_t xDuty)
{
	EnableMotors(1, 1, 1);
	motorPWMSetDutyCycle(xDuty, 0);
	//waitFunction(150);
	//DisableMotors();
}

void test_homing(int16_t xDuty)
{
	//mx_homing=1;
	uart_printf("start homing X\n");
	uart_printf("full pos %d, enc pos %d\n", mx_FullPos, getEncoderXPosition());
	//waitFunction(15);
	//EnableMotors();
	//waitFunction(10);
	//setMXSpeed(-mx_HomingSpeed);

	int homeswitch=GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z);
	uart_printf("EX: 0x%x\n", homeswitch);
	if(homeswitch==1)	// check if endswitch is not already activated
		{
		// if so move a little bit to the right direction
		uart_printf("Move right\n");

		EnableMotors();
		motorPWMSetDutyCycle(xDuty,0);	//z speed should be 0. you dont want to move that way, X low

		//test_moveXMotor(xDuty);
		for(double i=0;i<1000000;i++)
		{
			//do nothing replacement of the wait
		}
		//motorPWMSetDutyCycle(0,0);
		}

	EnableMotors();
	motorPWMSetDutyCycle(-xDuty,0); //still only moving in the x direction slowly
	//test_moveXMotor(-xDuty);

	while(GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z)==0)
	{// move until the end switch is triggered
		//direction to the left
		//waitFunction(50);
		homeswitch = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z);
		uart_printf("EX: 0x%x, DUTY: 0x%x, move left\n", homeswitch, xDuty);
	}
	homeswitch=1;

	uart_printf("finished homing x\n");

	DisableMotors(1, 1, 1);
	//mx_EncPosOld= getEncoderXPosition();

	//mx_homing=0;
}
void test_movement()
{
	uart_printf("before wait full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
	uart_printf("start test movement \n");
	initTimer7Interrupt();
	waitFunction(100);
	mx_FullPos = -13*Mem_PositionConstant;
	uart_printf("after wait full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
	EnableMotors();
	moveToPos(MX_HOME, MZ_DUMP_STACK+30, 1);
	setHorizontalVerticalServo(SERVO_VERT);
	waitFunction(50);
	for (int i=0; i<10; i++)
		{
		moveToPos(mx_storePos[0], MZ_DUMP_STACK, 1);
		waitFunction(50);
		uart_printf("dump1 \n");
		moveToPos(mx_storePos[0],MZ_DUMP_STACK+40, 1);
		waitFunction(50);
		moveToPos(mx_storePos[1], MZ_DUMP_STACK,1);
		waitFunction(50);
		uart_printf("dump2 \n");
		moveToPos(mx_storePos[1], MZ_DUMP_STACK+40,1);
		waitFunction(50);
		moveToPos(mx_storePos[2], MZ_DUMP_STACK,1);
		waitFunction(50);
		uart_printf("dump3 \n");
		moveToPos(mx_storePos[2], MZ_DUMP_STACK+40,1);
		waitFunction(50);
		moveToPos(MX_FLIPPER, MZ_FLIPPER +5, 1);
		waitFunction(50);
		uart_printf("flipper \n");
		moveToPos(MX_FLIPPER, MZ_DUMP_STACK+20, 1);
		waitFunction(50);
		moveToPos(MX_DUMP_STACK, MZ_DUMP_STACK, 1);
		waitFunction(50);
		uart_printf("dumpstack\n");
		uart_printf("%d \n", i);
		}

	moveToPos(MX_HOME, MZ_DUMP_STACK +40, 1);
	waitFunction(50);
	uart_printf("end test movement \n");


	/*while(1)
	{
		//moveToPos(20,0,1);
		//uart_printf("move to x=20\n");
		//uart_printf("full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
		//waitFunction(40);
		moveToPos(0,0,1);
		uart_printf("move to x=0\n");
		waitFunction(200);
		uart_printf("full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
		moveToPos(mx_columnPos[1], MZ_DUMP_STACK+60, 1);
		waitFunction(200);
		uart_printf("full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
		moveToPos(mx_columnPos[7], MZ_DUMP_STACK+60, 1);
		waitFunction(200);
		uart_printf("full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
		}*/

	DisableMotors();
	/*while(1)
	{
		uart_printf("full pos %lf, old pos %lf,  enc pos %lf\n", mx_FullPos/Mem_PositionConstant, mx_EncPosOld/Mem_PositionConstant, getEncoderXPosition()/Mem_PositionConstant);
		waitFunction(50);
	}*/
}
