/*
 * homing.c

 *
 *  Created on: 19 mrt. 2018
 *      Author: jajansen
 */

#include "homing.h"
#include "vars.h"
#include "GPIO_init_functions.h"
#include "motorPIDs.h"
#include "motorControlInterrupt.h"
#include "servoControl.h"
#include "uart.h"
#include "motorpwm.h"
#include "encoder.h"

/**
 * HomeRobot: Procedure for homing the robot (manipulators) at startup or reset
 * Contents:
 *  1. set the speed of the motors to zero and enable them
 *  2. set the gripper so it most likely doens't hit anything
 *  3. home the x axis
 *  4. set the gripper more horizontal so the z axis can be homed
 *  5. home the z axis
 *
 *  ATTENTION::
 *  the homing can NOT be started from any point. the machine is unaware of its position and will start homing
 *  the user must be made aware of this in the user manual.
 *  homing can only be done when there is no obstacle in the x direction movement of the robot.
 *
 */
void HomeRobot()
{
	// Set motor power enable
	MotorPWMduty(0, 0);
	EnableMotors(0, 0, 1);

	Home_X();
	Home_Z();
}

/**
 * Home_X: homing of the x axis
 * contents:
 * 1: check if the homeswitch is allready pressed
 * 2: when the homeswitch is pressed move away from it with a set speed for a period of time
 * 3: read the homeswitch, and move to the switch for as long as it is not pressed
 * 4: when the switch is pressed, stop moving and disable the motor
 *
 * NOTE:
 * Homing is done before the overall timer is started, that is why the moving is done by
 * setting the pwm values directly.
 * Also the waitfunction is not yet available. That is why a forloop is used to wait.
 */
void Home_X()
{
	int homeswitch = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X);

	if (homeswitch == 1) // check if endswitch is not already activated
	{
		// if so move a little bit to the right direction
		MotorPWMduty(0, 0);
		EnableMotors(1, 0, 0);
		MotorPWMduty(8, 0);
		for (double i = 0; i < 1000000; i++)
		{
			//do nothing replacement of the wait
		}
	}

	MotorPWMduty(0, 0);
	EnableMotors(1, 0, 0);
	MotorPWMduty(-9, 0); //still only moving in the x direction slowly

	while (GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X) == 0)
	{ // move until the end switch is triggered
		//direction to the left
		homeswitch = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X);
	}

	DisableMotors(1, 0, 0);
}


/**
 * Home_Z
 * Content:
 * 1: check if the homeswitch is allready pressed
 * 2: when the homeswitch is pressed move away from it with a set speed for a period of time
 * 3: read the homeswitch, and move to the switch for as long as it is not pressed
 * 4: when the switch is pressed, stop moving and disable the motor
 *
 * NOTE:
 * Homing is done before the overall timer is started, that is why the moving is done by
 * setting the pwm values directly.
 * Also the waitfunction is not yet available. That is why a forloop is used to wait.
 */
void Home_Z()
{
	int homeswitch = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z);

	if (homeswitch == 1) // check if endswitch is not already activated
	{
		// if so move a little bit upwards
		MotorPWMduty(0, 0);
		EnableZMotors();
		MotorPWMduty(0, 30); // x speed is 0, we dont want to move in that direction. z speed is high because of the friction in the lower part of the z axis.
		for (double i = 0; i < 50000; i++)
		{
			//do nothing replacement of the wait
		}
	}
	MotorPWMduty(0, 0);
	EnableZMotors();
	MotorPWMduty(0, -7); //still only moving in the z direction slowly

	while (GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z) == 0)
	{ // move until the end switch is triggered
		//direction downwards
		homeswitch = GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_Z);
	}

	DisableMotors(0, 1, 0);
}


/**
 * initHomeVar
 * Content:
 * to make sure the homing is updated to the encode the encoder position of home is written to EncPosOld.
 * by doing this the move to pos function will update the position relative to home.
 * the full pos is updated so it knows what the encoder value is of home.
 *
 */
void initHomeVar()
{
	mx_EncPosOld = getEncoderXPosition();
	mx_FullPos = HomeXPos * Mem_PositionConstant;
	mz_EncPosOld = getEncoderZPosition();
	mz_FullPos = HomeZPos * Mem_PositionConstant;
}
