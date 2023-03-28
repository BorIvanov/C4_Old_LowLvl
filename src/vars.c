/*
 * vars.c
 *
 *  Created on: 8 Sep 2017
 *      Author: chanselaar
 */
#include "vars.h"
#include "gameplay.h"
#include "uart.h"

void initGlobalVars(void)
{
	// GLOBAL VARIABLES
	// Motor control
	mx_EncPosOld = 0;	 	// old position on encoder motor x
	mz_EncPosOld = 0;	  	// old position on encoder motor z
	mx_EncSpeedPosOld = 0; 	// old position on encoder motor x for speed PID
	mz_EncSpeedPosOld = 0; 	// old position on encoder motor z for speed PID
	mx_PosTarget = 0;		// position target for encoder motor x
	mz_PosTarget = 0;	 	// position target for encoder motor z
	mx_PosTarget_Eff = 0; 	// effective target after limiting step size of target with maxDistStep
	mz_PosTarget_Eff = 0; 	// effective target after limiting step size of target with maxDistStep
	mx_FullPos = 0;		  	// full position motor x, updated at interrupt
	mz_FullPos = 0;		  	// full position motor z, updated at interrupt
	deltaX = 0;				// max value of the overflow
	deltaZ = 0;				// max value of the overflow
	overflowX = 0; 			// overflow/underflow of the encoder
	overflowZ = 0; 			// overflow/underflow of the encoder

	// motor x position PID values
	mx_positionPID_Kp = 0.11722; 	// proportional PID component
	mx_positionPID_Ki = 0.10655; 	// (was 0.10655) differential PID component, positive is real damping, negative is destabilation
	mx_positionPID_Kd = 0.022241; 	// (was 0.022241) note that the I control does have a dump if passing through the setpoint.

	// motor z position PID values		Check if they are used!!
	mz_positionPID_Kp = 0.008;  	// proportional PID component (was 0.008)
	mz_positionPID_Ki = 0.020;  	// (was 0.016) note that the I control does have a dump if passing through the setpoint.
	mz_positionPID_Kd = 0.0000; 	// (was 0.0008) differential PID component, positive is real damping, negative is destabilation

	mx_PIDIntCnt = 0;		 		// integral counter motor x
	mz_PIDIntCnt = 0;		 		// integral counter motor z
	PID_MaxIntWindup = 8000; 		// max integral counter, was 8000

	// SPEED CONTROL AND RAMP VARS
	mz_MinMotorSpeed = -1000; // minimum motor speed in rpm@motor shaft
	mz_MaxMotorSpeed = 1000; // maximum permissible motor speed in rpm@motor shaft
	mx_ismoving = 0;		 // variable to assess how the speed ramp is calculated (0=still, 1=CW, 2=CCW)
	mz_ismoving = 0;		 // variable to assess how the speed ramp is calculated (0=still, 1=CW, 2=CCW)

	// Counters
	TIM7IQ_ItCounter = 0; // iteration counter for function choice.
	TIM7_CustomTimerCounter = 0;

	// Other sensors and actuators
	Sens_CoinEntry = 0; // did we get a coin? 0 = no, 1:7 is entry point, 9 for error
	gateOld = -1;

	// translation of mm to encoder pulses
	Mem_mmPerRev = 50;				// Spindle mm / revolution of gearbox
	Mem_motorGearing = 27.6;		// Motor gear box ratio
	Mem_EncoderPulsesPerRev = 2000; // Number of pulses given by encoder per motor shaft revolution
	Mem_PositionConstant = (Mem_EncoderPulsesPerRev * Mem_motorGearing) / Mem_mmPerRev;
	/**
	 * Calculation of positioning factor between encoder counts and linear movement
	 *  One gearbox output shaft rotation = 50mm translation
	 *  One gearbox output shaft rotation requires 27.6 motor shaft rotations
	 *  One motor shaft rotation equals 2000 encoder pulses
	 */

	// homing variables			found through testing
	// when a new home switch or attachment is used the variable can change
	HomeXPos = -13;		// distance from the home button to the zero point
	HomeZPos = -25;		// distance from the home button to the zero point

	// number of coins per column current and old memory
	mem_Board[0] = 0; // board layout
	mem_Board[1] = 0; // board layout
	mem_Board[2] = 0; // board layout
	mem_Board[3] = 0; // board layout
	mem_Board[4] = 0; // board layout
	mem_Board[5] = 0; // board layout
	mem_Board[6] = 0; // board layout

	mem_Board_old[0] = 0; // board layout
	mem_Board_old[1] = 0; // board layout
	mem_Board_old[2] = 0; // board layout
	mem_Board_old[3] = 0; // board layout
	mem_Board_old[4] = 0; // board layout
	mem_Board_old[5] = 0; // board layout
	mem_Board_old[6] = 0; // board layout

	// positions of the empty servo for each of the columns to open.
	// empty[0] is closing of the slide
	// values are found through testing
	empty[0] = EMPTYSERVOCLOSED + EMPTYSERVO1;
	empty[1] = EMPTYSERVOCLOSED + EMPTYSERVO2;
	empty[2] = EMPTYSERVOCLOSED + EMPTYSERVO3;
	empty[3] = EMPTYSERVOCLOSED + EMPTYSERVO4;
	empty[4] = EMPTYSERVOCLOSED + EMPTYSERVO5;
	empty[5] = EMPTYSERVOCLOSED + EMPTYSERVO6;
	empty[6] = EMPTYSERVOCLOSED + EMPTYSERVO7;

	//array used for order of emptying columns (column numbers from the left seen from robot side)
	empty_mem[0] = 5;
	empty_mem[1] = 1;
	empty_mem[2] = 0;
	empty_mem[3] = 3;
	empty_mem[4] = 6;
	empty_mem[5] = 2;
	empty_mem[6] = 4;

	// mem of coins in stack
	mem_StorageStack[0] = 7; 	// storage, starts filled during game start!
	mem_StorageStack[1] = 7;	// storage, starts filled during game start!
	mem_StorageStack[2] = 7;	// storage, starts filled during game start!
	StackChoice = 0;			// storage stack used to pick up a coin.

	play_ActionCompleted = 1;
	play_EndGame = 2;

	state = STATE_BOOTUP;		// set state to bootup
	uartStartSend = 0;	 		// initialize UART Start Send
	uartStopSend = 0;	  		// initialize UART Stop Send
	uartIndex = 0;		   		// initialize UART Index
	processingCommand = 0;		// initialize processingCommand
	ackNackReceived = 0;   		// initialize ackNackReceived
	ackValue = 0;		   		// initialize ackValue

	// Column and stack positions
	mx_columnPos[1] = MX_HOME + MX_COLUMN1;
	mx_columnPos[2] = MX_HOME + MX_COLUMN2;
	mx_columnPos[3] = MX_HOME + MX_COLUMN3;
	mx_columnPos[4] = MX_HOME + MX_COLUMN4;
	mx_columnPos[5] = MX_HOME + MX_COLUMN5;
	mx_columnPos[6] = MX_HOME + MX_COLUMN6;
	mx_columnPos[7] = MX_HOME + MX_COLUMN7;

	// left, middle and right store stack
	mx_storePos[0] = MX_HOME + MX_STOREPOS0;
	mx_storePos[1] = MX_HOME + MX_STOREPOS1;
	mx_storePos[2] = MX_HOME + MX_STOREPOS2;

	// storepos z position
	mz_storePos[0] = MZ_STORE - MZ_STOREPOS0; //lowest chip
	mz_storePos[1] = MZ_STORE - MZ_STOREPOS1;
	mz_storePos[2] = MZ_STORE - MZ_STOREPOS2;
	mz_storePos[3] = MZ_STORE - MZ_STOREPOS3;
	mz_storePos[4] = MZ_STORE - MZ_STOREPOS4;
	mz_storePos[5] = MZ_STORE - MZ_STOREPOS5;
	mz_storePos[6] = MZ_STORE - MZ_STOREPOS6 - 3; // highest chip	// -3 is found through testing. by adding this factor all the coins are picked up okay.

	// dumppos z position
	mz_dumpPos[0] = MZ_DUMP_STACK - MZ_DUMPPOS0;
	mz_dumpPos[1] = MZ_DUMP_STACK - MZ_DUMPPOS1;
	mz_dumpPos[2] = MZ_DUMP_STACK - MZ_DUMPPOS2;
	mz_dumpPos[3] = MZ_DUMP_STACK - MZ_DUMPPOS3;
	mz_dumpPos[4] = MZ_DUMP_STACK - MZ_DUMPPOS4;
	mz_dumpPos[5] = MZ_DUMP_STACK - MZ_DUMPPOS5;


	sens = 1;	// variable is 1 when he is waiting on a human coin

	/*variables used in endurance test*/
	rgb_error = 0;
	detect_error = 0;
	input_error = 0;
	geel = 0;
	rood = 0;

	servoPosition = 0;
	moveServo = 0;
}
