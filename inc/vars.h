/*
 * vars.h
 *
 *  Created on: 8 Sep 2017
 *      Author: chanselaar
 */

#ifndef VARS_H_
#define VARS_H_

#include "stm32f30x.h" // base board parameters and settings --> adjusted HSE_VALUE

/* DEFINES */
#define LED1 GPIO_Pin_8
#define LED2 GPIO_Pin_9
#define LED3 GPIO_Pin_10
#define LED4 GPIO_Pin_11
#define LED5 GPIO_Pin_12
#define LED6 GPIO_Pin_13
#define LED7 GPIO_Pin_14
#define LED8 GPIO_Pin_15
#define LEDS (LED1 | LED2 | LED3 | LED4 | LED5 | LED6 | LED7 | LED8)
#define DLED1 GPIO_Pin_15
#define DLED2 GPIO_Pin_14
#define DLED3 GPIO_Pin_13
#define DLED4 GPIO_Pin_12
#define DLEDS (DLED1 | DLED2 | DLED3 | DLED4)
#define DLED_PORT GPIOB
#define BUTTON GPIO_Pin_0
#define E_STOP_PORT GPIOC
#define E_STOP_PIN GPIO_Pin_1
#define RST_BUTTON_PORT GPIOA
#define RST_BUTTON_PIN GPIO_Pin_4
#define HOME_END_PORT GPIOB
#define HOMING_X GPIO_Pin_10
#define HOMING_Z GPIO_Pin_1
#define END_X GPIO_Pin_0
#define END_Z GPIO_Pin_2
#define MX_PWM_PIN GPIO_Pin_9
#define MX_PWM_PORT GPIOA
#define MX_PWM_PINSOURCE GPIO_PinSource9
#define MX_PWM_AF GPIO_AF_6
#define MX_DIR_PIN GPIO_Pin_10
#define MX_DIR_PORT GPIOA
#define MX_EN_PIN GPIO_Pin_11
#define MX_EN_PORT GPIOA
#define MZ_PWM_PIN GPIO_Pin_8
#define MZ_PWM_PORT GPIOA
#define MZ_PWM_PINSOURCE GPIO_PinSource8
#define MZ_PWM_AF GPIO_AF_6
#define MZ_DIR_PIN GPIO_Pin_9
#define MZ_DIR_PORT GPIOC
#define MZ_EN_PIN GPIO_Pin_7
#define MZ_EN_PORT GPIOC
#define VAC_PUMP_PIN GPIO_Pin_3
#define VAC_PUMP_PORT GPIOA
#define VAC_VALVE_PIN GPIO_Pin_0
#define VAC_SENSE GPIO_Pin_5
#define VAC_VALVE_PORT GPIOC
#define Mem_HalfUint 32767 // half the uint range
#define NumberOfBoardColumns 7
#define NumberOfBoardRows 6
#define NumberOfStorages 3

/* GLOBAL VARIABLES */
// Motor control
// check if this have to be doubles
double mx_EncPosOld;        // old position on encoder motor 1 (x direction)
uint16_t mz_EncPosOld;      // old position on encoder motor 2 (z direction)
uint16_t mx_EncSpeedPosOld; // old position on encoder motor 1 (x direction)
uint16_t mz_EncSpeedPosOld; // old position on encoder motor 2 (z direction)
int32_t mx_PosTarget;       // position target for encoder motor 1
int32_t mz_PosTarget;       // position target for encoder motor 2
int32_t mx_HalfPosTarget;   // halfway position of encoder for motor 1 (x direction)
int32_t mz_HalfPosTarget;   // halfway position of encoder for motor 2 (z direction)
int32_t mx_PosTarget_Eff;   // effective target after limiting step size of target with maxDistStep
int32_t mz_PosTarget_Eff;   // effective target after limiting step size of target with maxDistStep
double mx_FullPos;          // full position m1, updated at interrupt
int32_t mz_FullPos;         // full position m2, updated at interrupt
int sens;


double deltaX; 				// max value of overflow
double deltaZ; 				// max value of overflow
int overflowX; 				// over/underflow of encoder | -1 for underflow, 1 for overflow.
int overflowZ; 				// over/underflow of encoder | -1 for underflow, 1 for overflow.

// motor x position PID values
double mx_positionPID_Kp; 	// proportional PID component
double mx_positionPID_Kd; 	// differential PID component, positive is real damping, negative is destabilation
double mx_positionPID_Ki; 	// note that the I control does have a dump if passing through the setpoint.

// motor z position PID values
double mz_positionPID_Kp; 	// proportional PID component
double mz_positionPID_Kd; 	// differential PID component, positive is real damping, negative is destabilation
double mz_positionPID_Ki; 	// note that the I control does have a dump if passing through the setpoint.

double mx_PIDIntCnt;     // integral counter motor 1
double mz_PIDIntCnt;     // integral counter motor 2
double PID_MaxIntWindup; // max integral counter for speed PID

// JJ SPEED CONTROL VARS
int32_t mx_RampMotorSpeed; 	// calculated permissible mx speed during acceleration
int32_t mz_MaxMotorSpeed; 	// maximum permissible speed of motor z
int32_t mz_MinMotorSpeed; 	// minimum speed of motor z

// TESTING VARS JJ
double mx_acceleration; 	// mx rpm step added during each iteration of calcAccelration
double mz_acceleration; 	// mz rpm step added during each iteration of calcAccelration
int mx_ismoving;        	// direction of mx | 1=CW, -1=CCW, 0=StandStill
int mz_ismoving;        	// direction of mx | 1=CW, -1=CCW, 0=StandStill

// Counters
uint16_t TIM7IQ_ItCounter; // iteration counter for function choice.
uint32_t TIM7_CustomTimerCounter; // timer  counter

// Other sensors and actuators
int Sens_CoinEntry; // did we get a coin? 0 = no, 1:7 is entry point, 9 for error
int gateOld;		// Saves which gate the last thrown in coin was in.

// global variables that don't need to be reassigned after assemble
// initializing these commands
double Mem_mmPerRev;            // spindle mm / rev
double Mem_motorGearing;        // motor gearbox
double Mem_EncoderPulsesPerRev; // number of pulses given by encoder per motor shaft revolution
double Mem_PositionConstant;    // positioning constant to convert encoder value to linear translation
double HomeXPos;				// distance from 0 to home in the x direction
double HomeZPos;				// distance from 0 to home in the z direction
int mem_Board[NumberOfBoardColumns];     // board layout
int mem_Board_old[NumberOfBoardColumns]; // board layout
int empty[7];                            //empty servo pos
int empty_mem[7];
unsigned int mem_StorageStack[NumberOfStorages]; // storage, starts filled during game start!
unsigned int StackChoice;
int play_ActionCompleted;
int play_EndGame;

// communication commands
uint8_t state;
uint8_t processingCommand;

//Endurance test
uint8_t rgb_error;    //rgb/pickup fails
uint8_t detect_error; //coindetect/input fails
uint8_t input_error;  //input_error
uint8_t geel;         //input_error
uint8_t rood;         //input_error

int input_test;
int robotMove;

// X positions all related to home. Home is the left column (seen from the side of the robot)
#define MX_HOME 0                     	// X positon of home/travel
#define MX_FLIPPER (MX_HOME + 158)    	// X position of the flipper/rgb sensor
#define MX_DUMP_STACK (MX_HOME + 322) 	// X position of the dump stack base
#define MX_SLIDER (MX_HOME + 85)      	// X position for the slider???	// not used anywhere
#define MX_SLIDER_PITCH 35            	// X position between columns 	// not used anywhere but useful to know
// left, middle and right store stack
#define MX_STOREPOS0 4					// left store stack
#define MX_STOREPOS1 37
#define MX_STOREPOS2 71					// right store stack
// Column positions
#define MX_COLUMN1 -4
#define MX_COLUMN2 30
#define MX_COLUMN3 65
#define MX_COLUMN4 100
#define MX_COLUMN5 134
#define MX_COLUMN6 169
#define MX_COLUMN7 203

// Z positions all related to home. Home is the
#define MZ_HOME 0                    	// Z position of home/travel height
#define MZ_TOP (MZ_HOME + 335)        	// Z position above board, where the rotation can be made
#define MZ_DROP (MZ_HOME + 310)       	// Z position where the coin can be dropped into the board | TODO: check this position
#define MZ_FLIPPER (MZ_HOME + 6)      	// Z position of flipper/rgb sensor
#define MZ_DUMP_STACK (MZ_HOME + 82)  	// Z position of dump stack base		// was 62
#define MZ_STORE (MZ_HOME + 45)       	// Z position of store stack base | TODO: check if this is the proper position
#define MZ_CHIP_HEIGHT 7              	// Z height of chip in mm
//z position store
#define MZ_STOREPOS0 42
#define MZ_STOREPOS1 (MZ_STOREPOS0 - 1 * MZ_CHIP_HEIGHT)
#define MZ_STOREPOS2 (MZ_STOREPOS0 - 2 * MZ_CHIP_HEIGHT + 1)	// added an extra mm, to many problems picking up the coin without.
#define MZ_STOREPOS3 (MZ_STOREPOS0 - 3 * MZ_CHIP_HEIGHT)
#define MZ_STOREPOS4 (MZ_STOREPOS0 - 4 * MZ_CHIP_HEIGHT)
#define MZ_STOREPOS5 (MZ_STOREPOS0 - 5 * MZ_CHIP_HEIGHT)
#define MZ_STOREPOS6 (MZ_STOREPOS0 - 6 * MZ_CHIP_HEIGHT)
// z positions of dumpstack
#define MZ_DUMPPOS0 113 //used to be 88
#define MZ_DUMPPOS1 (MZ_DUMPPOS0 - 1 * MZ_CHIP_HEIGHT - 1)
#define MZ_DUMPPOS2 (MZ_DUMPPOS0 - 2 * MZ_CHIP_HEIGHT - 1)
#define MZ_DUMPPOS3 (MZ_DUMPPOS0 - 3 * MZ_CHIP_HEIGHT - 1)
#define MZ_DUMPPOS4 (MZ_DUMPPOS0 - 4 * MZ_CHIP_HEIGHT - 1)
#define MZ_DUMPPOS5 (MZ_DUMPPOS0 - 5 * MZ_CHIP_HEIGHT - 1)


// servo position to empty the columns
#define EMPTYSERVOCLOSED 550 	//higher than the others because servo 1..7 are added to 0
#define EMPTYSERVO1 210
#define EMPTYSERVO2 330
#define EMPTYSERVO3 590
#define EMPTYSERVO4 710
#define EMPTYSERVO5 1210
#define EMPTYSERVO6 1340
#define EMPTYSERVO7 1710

// x constants
double mx_columnPos[NumberOfBoardColumns + 1]; // column position array
double mx_storePos[NumberOfStorages];          // storage positions array

// z constants
double mz_storePos[7];
double mz_dumpPos[NumberOfBoardRows];

// min and max values of x and z. Both can have a position below zero, because 0 is the lowest point of the game, not of its actual axis.
#define MX_LIMIT_LOW -10
#define MX_LIMIT_HIGH 500
#define MZ_LIMIT_LOW -30
#define MZ_LIMIT_HIGH 335

// gripper variables
#define SERVO_VERT 90   // vertical angle for end of arm servo
#define SERVO_HORZ 175  // horizontal angle for end of arm servo
#define GRIPPER_RELEASE_ANGLE 170 // Gripper angle at which coins are released
#define GRIPPER_HOMING_ANGLE_SAFE 150 // Safe angle for gripper during homing procedure so it has the least chance it will hit something when homing the X axis.

int servoPosition;
int moveServo;

#define ROBOT_CHIP 1	// Red
#define HUMAM_CHIP 0	// Yellow

void initGlobalVars(void);

#endif /* VARS_H_ */
