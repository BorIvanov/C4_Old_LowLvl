#include "init.h"
#include "stm32f30x.h"			   // Base board parameters and settings --> adjusted HSE_VALUE
#include "checkForCoinPlay.h"	  // Check if coins enter the system
#include "vars.h"				   // Defining global variables
#include "initSysClock.h"		   // System clock settings, to make sure 72 MHz are attained
#include "GPIO_init_functions.h"   // GPIO initialization functions, to assign out and inputs.
#include "motorpwm.h"			   // Motor PWM controller (DAC)
#include "encoder.h"			   // Motor Encoder readings, for x and z control
#include "vacControl.h"			   // Perform checks for vacuum control
#include "rgb_light_control.h"	 // Turn on and check colour of system
#include "solenoidControl.h"	   // Control the flipper
#include "proximityTest.h"		   // Check if coins remain in stack
#include "uart.h"				   // UART communication with PI
#include "i2c.h"				   // I2C communication for sensors
#include "gamePlay.h"			   // Game play commands
#include "readDifficulty.h"		   // Read difficulty
#include "servoControl.h"		   // Control servos
#include "motorControlInterrupt.h" // Custom timers, ranging from interrupts to PWM's and DAC's
#include "motorPIDs.h"			   // Main PID loops for x and z control
#include "homing.h"				   // Homing sequence and routines

/* TESTING INCLUDES */
#include "../test/hwtest_Main.h"

void initProgram(void)
{
	/* INITIALIZATION FUNCTIONS */
	initGlobalVars();	  // Initialization of global variables and startup state
	initSysClock();		   // Initialization of the system clock at correct rate
	initEncoder();		   // Initialization of the X/Z motor encoders
	initMotorPWM();		   // Start the PWM logic.
	initPump();			   // Initialization of the Vacuum pump
	initValve();		   // Initialization of the Vacuum valve
	initVacSens();		   // Initialization of vacuum sensor, to be done after interrupt enable
	initTimer7();		   // Initialization of the timer
	initGPIOLEDs();		   // Initialization of the GPIO LEDS
	GPIO_SetBits(DLED_PORT, DLEDS);
	initSolenoid();		   // Initialization of the 'flipper' solenoid
	initServoPWMs();	   // Initialization of the servos
	//initRotarySwitch();	// Initialization of inputs for the rotary switch. (not yet physically implemented)
	initHomeSwitches();	// Initialization of for home switches
	initEStop();		   // Initialization of the Emergency Stop input signal
	//initEndSwitches();	 // Initialization of for end switches (not yet physically implemented)
	initI2C();			   // Initialization of I2C communication protocol
	initCD();			   // Initialization of the coin detector
	initTimer3Interrupt(); // Initialization of timer3 Interrupts (enable)
	initTimer4Interrupt(); // Initialization of timer4 Interrupts (enable)

	HomeRobot();		   // Home all actuators (x/z and servo(s))
	initUART();			   // Initialize UART communication protocol

	//.test_Main();			// => here for homing tests

	initTimer7Interrupt(); // Initialization of timer7 interrupts (enable)
	initHomeVar();		   // Initialization of the last homing variables
	initRGBSensor();	   // Initialization of the rgb detector

	//.test_Main();			// uncomment to run tests

	//NOTE: comments with //. are the ones commented for testing
}
