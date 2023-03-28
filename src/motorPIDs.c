/*
 * motorPIDs.c
 *
 *  Created on: 31 Aug 2017
 *      Author: chanselaar
 */
#include "vars.h"
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "motorpwm.h"
#include "encoder.h"
#include "motorPIDs.h"
#include "gameplay.h"
#include "uart.h"
#include "motorControlInterrupt.h"
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>

/**
 * unwrapEncDelta: unwrap the encoder position
 *
 * Since the encoder position is stored as an unsigned integer,
 * a jump is made from zero to the max. integer value when it would otherwise become negative.
 * This function fixes that error in the delta position
 */
int32_t unwrapEncDelta(int32_t delta)
{
	// perform logic to unwrap, since encoder position is unsigned int
	if (delta > Mem_HalfUint)
	{ // If the position change is more than half uint, wrap
		delta -= (65535 + 1);

	} // wrap by increasing the old position
	else if (delta < -Mem_HalfUint)
	{ // If the position change is more than half uint, wrap
		delta += (65535 + 1);

	} // wrap by decreasing the old position

	return delta;
}

/**
 * clampValue: clamp a value between negative and positive intger
 *
 */
double clampValue(double value, double minvalue, double maxvalue)
{
	return value <= minvalue ? minvalue : (value >= maxvalue ? maxvalue : value);
}


/**
 * PID: calculate PID
 *
 * Calculates PID output.
 *
 * Parameters:
 * 	Kp: proportional gain
 * 	Kd: differential gain
 * 	Ki: integral gain
 * 	period: period time in seconds
 * 	err: position error
 * 	delta: position change since last cycle
 * 	intCount: pointer to the integral value
 */
double PID(double Kp, double Kd, double Ki, double period, int32_t err,
		   int32_t delta, double *intCount)
{
	/* Initialize local variables */
	double vel = 0;
	double p_V = 0;
	double d_V = 0;
	double i_V = 0;

	/* Calculate velocity */
	vel = -((double)delta) / period;

	/* Integrate error */
	*intCount += (double)err * period;

	/* Limit integral windup */
	if (*intCount > PID_MaxIntWindup)
		*intCount = PID_MaxIntWindup;
	else if (*intCount < -PID_MaxIntWindup)
		*intCount = -PID_MaxIntWindup;

	/* 4. execute PID */
	p_V = Kp * (double)err; // proportional position request
	d_V = Kd * vel;			// differential position request
	i_V = Ki * *intCount;   // integral position request

	/* 5. set demand */
	return p_V + d_V + i_V;
}

/**
 * motorPIDControl: control loop for position of motor X and Z
 * Contents
 *   1. Read current encoder position
 *   2. Compare to old position of encoder for m1 and m2, and compute delta and speed of position
 *   3. Update with distance traveled since last interrupt, and save into fullPos
 *   4. Execute PID loop
 *   	a. Compute difference of target position - global position, and multiply with kp
 *   	b. Compute velocity via pos_global_old, new global postion, and time between interrupt,
 *   	    and multiply by kd
 *   	c. Compute integral
 *   5. Set demand for speed based on sum of PID components
 */
void motorPIDPositionControl()
{
	/* Initialize local variables */
	double encPosCur_mx = 0; // Current m1 position
	double encPosCur_mz = 0; // Current m2 position
	double delta_mx = 0;
	double delta_mz = 0;
	double posError_mx = 0;
	double posError_mz = 0;

	deltaX = 65535 + 1;
	deltaZ = 65535 + 1;
	double mx_speedDem;
	double mz_speedDem;
	double period = 0.01; // Position control runs at F=100Hz --> T=0.01s

	/* MOTOR X */
	deltaX = deltaX * overflowX;
	overflowX = 0;
	encPosCur_mx = getEncoderXPosition();			 // Check the positions
	delta_mx = encPosCur_mx + deltaX - mx_EncPosOld; // Calculate position delta
	mx_EncPosOld = encPosCur_mx;					 // Update old position
	mx_FullPos = mx_FullPos + delta_mx;				 // Update global position
	posError_mx = mx_PosTarget - mx_FullPos;		 // Define delta to target

	/* Set speed demand */
	mx_speedDem = (PID(mx_positionPID_Kp, mx_positionPID_Kd, mx_positionPID_Ki, period, posError_mx, delta_mx, &mx_PIDIntCnt));

	/* MOTOR Z */
	deltaZ = deltaZ * overflowZ;
	overflowZ = 0;
	encPosCur_mz = getEncoderZPosition();			 // Check the positions
	delta_mz = encPosCur_mz + deltaZ - mz_EncPosOld; // Calculate position delta
	mz_EncPosOld = encPosCur_mz;					 // Update old position
	mz_FullPos = mz_FullPos + delta_mz;				 // Update global position
	posError_mz = mz_PosTarget - mz_FullPos;		 // Define delta to target

	/* 5. set speed demand */
	mz_speedDem = PID(mz_positionPID_Kp, mz_positionPID_Kd, mz_positionPID_Ki,
					  period, posError_mz, delta_mz, &mz_PIDIntCnt);

	/* limit setspeed of motor Z*/
	mz_speedDem = clampValue(mz_speedDem, mz_MinMotorSpeed, mz_MaxMotorSpeed);

	motorPWMSetDutyCycle((int)mx_speedDem, (int)mz_speedDem);

} // end of position PID handler

/*
 * PID helper functions
 */
void setControllerPID(double Kp, double Ki, double Kd, double max_speed)
{
	mz_positionPID_Kp = Kp;		  // proportional PID component (was 0.008)
	mz_positionPID_Ki = Ki;		  // (was 0.016) note that the I control does have a dump if passing through the setpoint.
	mz_positionPID_Kd = Kd;		  // (was 0.0008) differential PID component, positive is real damping, negative is destabilation
	mz_MaxMotorSpeed = max_speed; // maximum permissible motor speed in rpm@motor shaft
	mz_PIDIntCnt = 0;			  // reset integrator
}

/*
 * Since the downwards movement is not a linear one, it is (with my knowledge) not possible to determine a PID set for the complete
 * downwards movement. Nor am I capable of calculating a PID set that works for both upwards and downwards movement. - Aniel Shri
 * To get a working system, a different PID value is chosen for different (groups of) setpoint(s).
 */
void selectZPID(int16_t targetZ)
{
	// don't do anything if moving upwards
	if (mz_ismoving == 1)
		return;

	if (targetZ >= MZ_DROP) // drop coin in matrix
	{
		setControllerPID(0.007, 0.005, 0.001, 500);
	}
	else if (targetZ >= MZ_DUMP_STACK)
	{
		setControllerPID(0.007, 0.005, 0.002, 500);
	}
	else if (targetZ >= mz_storePos[6]) // top stack coin
	{
		setControllerPID(0.002, 0.0015, 0.002, 500);
	}
	else if (targetZ >= mz_storePos[5]) // stack coin #6
	{
		setControllerPID(0.003, 0.0015, 0.002, 500);
	}
	else if (targetZ >= mz_storePos[4]) // stack coin #5
	{
		setControllerPID(0.005, 0.0015, 0.002, 500);
	}
	else if (targetZ >= mz_storePos[3]) // stack coin #4
	{
		setControllerPID(0.005, 0.0015, 0.002, 500);
	}
	else if (targetZ >= mz_storePos[0]) // stack coin #1-3
	{
		setControllerPID(0.007, 0.005, 0.002, 500);
	}
	else if (targetZ >= mz_dumpPos[4]) // dump coin #5-6
	{
		setControllerPID(0.007, 0.010, 0.002, 500);
	}
	else if (targetZ >= mz_dumpPos[3]) // dump coin #4
	{
		setControllerPID(0.007, 0.010, 0.002, 500);
	}
	else if (targetZ >= mz_dumpPos[1]) // dump coin #2-3
	{
		setControllerPID(0.008, 0.015, 0.002, 500);
	}
	else if (targetZ >= mz_dumpPos[0]) // dump coin #1
	{
		setControllerPID(0.007, 0.015, 0.001, 500);
	}
	else
	{
		setControllerPID(0.007, 0.015, 0.001, 500);
	}
}

/** moveToPos: set target for x and z motor and wait till it is attained
 *
 * Parameters:
 * 	target: target position in mm axis translation
 * 	maxDelta: acceptable range in precision
 */
void moveToPos(int16_t targetX, int16_t targetZ, double maxDelta)
{
	EnableMotors(0,1,0);
	mx_ismoving = calcDirection(mx_FullPos, targetX * Mem_PositionConstant);
	mz_ismoving = calcDirection(mz_FullPos, targetZ * Mem_PositionConstant);

	uint16_t nBounceCount = 0;
	uint16_t timeCount = 0;

	if(mx_ismoving != 0)
	{
		EnableMotors(1,0,0);
	}

	double pid_up[] = {0.018 * 2, 0.025 * 3, 0, 1000};

	if (mz_ismoving == -1)
	{
		selectZPID(targetZ);
	}
	else
	{
		setControllerPID(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
	}

	// when moving downwards => move x-axis first and then z-axis
	if (mz_ismoving == -1 && mx_ismoving != 0)
	{
		mx_PosTarget = targetX * Mem_PositionConstant;
		while (labs(mx_FullPos - mx_PosTarget) > maxDelta * Mem_PositionConstant)
		{
			// do nothing
		}
		DisableMotors(1, 0, 0);
	}

	if(mz_ismoving != 0)
	{
		mz_PosTarget = targetZ * Mem_PositionConstant;

		while (labs(mz_FullPos - mz_PosTarget) > 1.5 * Mem_PositionConstant)
		{
			if (labs(mz_FullPos - mz_PosTarget) < 1.5 * Mem_PositionConstant)
			{
				nBounceCount++;
				if (nBounceCount >= 2)
					break;
			}
			else if (timeCount > 70) // could not reach setpoint (timeout)
			{
				setControllerPID(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
				break;
			}
			else
			{
				nBounceCount = 0;
			}
		}
	}

	// when moving upwards => move x-axis after the z-axis
	if (mz_ismoving == 1 && mx_ismoving != 0)
	{
		mx_PosTarget = targetX * Mem_PositionConstant;
		while (labs(mx_FullPos - mx_PosTarget) > 1.5 * Mem_PositionConstant)
		{
			// do nothing
		}
		DisableMotors(1, 0, 0);
	}

	mx_ismoving = 0;
	mz_ismoving = 0;
}

/** calcDirection: calculate motor direction for motion path
 *
 * Parameters:
 * 	currentPos: current position in encoder pulses
 * 	targetPos: target position in encoder pulses
 */

int calcDirection(int32_t currentPos, int32_t targetPos)
{
	if(targetPos == currentPos)
		return 0;
	else if (targetPos > currentPos)
		return 1; // CW rotation
	else
		return -1; // CCW rotation
}
