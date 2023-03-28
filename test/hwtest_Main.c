/*hwtest_Main.c
 *
 *  Created on: 30 apr. 2018
 *      Author: jajansen
 */

#include "checkForCoinPlay.h"
#include "../test/hwtest_CoinDetector.c"
#include "../test/test_tunePIDs.h"
#include "../test/test_homing.h"

#include "stdlib.h"
#include "stm32f30x.h"
#include "vars.h"
#include "servoControl.h"
#include "motorpwm.h"
#include "motorPIDs.h"
#include "motorControlInterrupt.h"
#include "rgb_light_control.h"
#include "gamePlay.h"

#define NOTEST					0
#define CDCHANNELTEST			1
#define CDDEBOUNCETEST			2
#define ENDSWITCHESINPUT		3
#define ENDSWITCHESINTERRUPT	4
#define DIFFICULTYSWITCHINPUT	5
#define ESTOPINPUT				6
#define RESETSWITCHINPUT		7
#define RGBSENSORINPUT			8
#define SOLENOID				9
#define MOTORX					10
#define MOTORZ					11
#define SERVO					12
#define VACUUM					13
#define SYSTEM					14
#define SLIDER					15
#define ENDURANCE				16
#define CHECK_COIN				17

//int inputnew;
void test_CheckCoin(void);
void test_CoinDetectorChannelTest(void);
void test_CoinDetectorDebounceTest(void);
void test_EndSwitchesInterrupt(void);
void test_DifficultySwitchInput(void);
void test_EStopInput(void);
void test_ResetSwitchInput(void);
void test_RGBSensor(void);
void test_Solenoid(void);
void test_MotorX(void);
void test_Servo(void);
void test_Vacuum(void);
void test_Slider(void);
void test_endurance(void);

//extern void initialise_monitor_handles(void);
extern void test_PIDsMain(const char *testParam);
extern void test_HomingMain(void);

/* CHANGE THIS VARIABLE TO SELECT THE TEST TO RUN */

int testitem = CHECK_COIN;

/* ---------------------------------------------- */

void test_Main(void)
{
	// init
	//initialise_monitor_handles();
	enableWriteString = 1;
	uartWriteString("Welcome to test_Main\n");

	switch (testitem)
	{
		case CHECK_COIN:
			uartWriteString("Starting CHECK_COIN test...\n");
			test_CheckCoin();

			break;
		case CDCHANNELTEST:
			//	puts("Starting Coin Detector Channel test...\n");
			test_CoinDetectorChannelTest();

			break;
		case CDDEBOUNCETEST:
			puts("Starting Coin Detector Debounce test...\n");
			test_CoinDetectorDebounceTest();

			break;
		case ENDSWITCHESINPUT:
			puts("Starting Home & End Switch input test...\n");
			//test_EndSwitchesInput();
			test_HomingMain();

			break;
		case ENDSWITCHESINTERRUPT:
			puts("Starting Home & End Switch interrupt test...\n");
			test_EndSwitchesInterrupt();

			break;
		case DIFFICULTYSWITCHINPUT:
			puts("Starting difficulty switch input test...\n");
			test_DifficultySwitchInput();

			break;
		case ESTOPINPUT:
			puts("Starting emergency stop input test...\n");
			test_EStopInput();

			break;
		case RESETSWITCHINPUT:
			puts("Starting reset switch input test...\n");
			test_ResetSwitchInput();

			break;
		case RGBSENSORINPUT:
			uartWriteString("Starting RGB Sensor input test...\n");
			test_RGBSensor();

			break;
		case SOLENOID:
			uartWriteString("Starting solenoid test...\n");
			test_Solenoid();

			break;
		case MOTORX:
			uartWriteString("Starting motorX test...\n");
			test_MotorX();

			break;
		case MOTORZ:
			uartWriteString("Starting motorZ test...\n");
			test_PIDsMain("z");
			//test_MotorZGamePlay();
			//test_MotorZ();

			break;
		case SERVO:
			uartWriteString("Starting SERVO test...\n");
			test_Servo();

			break;
		case VACUUM:
			uartWriteString("Starting VACUUM test...\n");
			test_Vacuum();

			break;
		case SLIDER:
			uartWriteString("Starting SLIDER test...\n");
			test_Slider();

			break;
		case ENDURANCE:
			uartWriteString("Starting ENDURANCE test...\n");
			test_endurance();

			break;
		default:
			puts("Nothing to test...");
			break;
	}
	uartWriteString("Done");    // make sure we stop here
}

void test_CheckCoin(void)
{
	uint8_t columnChanged;

	while(1)
	{
		uartWriteString("Wait for coin..\n");
		while (checkcoin() == -1)
		{
		} //wait for coin
		columnChanged = checkcoin();
		uartWriteString("Coin is tossed..\n");
		uart_printf("Coin tossed in column: %d\n\n", columnChanged);

		waitFunction(500);

		columnChanged = checkcoin();
		if (columnChanged == 9 || detect_error != 0)
		{
			uartWriteString("Cheater!\n\n");
			detect_error = 0;
		}

		for (int k = 0; k < 7; k++)
		{
			mem_Board_old[k] = mem_Board[k];
		} //update board
	}
}

/* test_CoinDetectorChannelTest
 *
 * Procedure that reads out the variable controlled by the coin detector
 * Only used for testing, outputs to the console when semihosting is enabled
 *
 */
void test_CoinDetectorChannelTest(void)
{
	sens = 1;
	uartWriteString("test_CoinDetectorChannelTest Start\n");
	//printf("\start channel test\n\n" );
	while (1)
	{

//		printf("%f \n",mem_Board[0]);
//		printf("%f \n",mem_Board[1]);
//		printf("%f \n",mem_Board[2]);
//		printf("%f \n",mem_Board[3]);
//		printf("%f \n",mem_Board[4]);
//		printf("%f \n",mem_Board[5]);
//		printf("%f \n",mem_Board[6]);
//		waitFunction(500);

		int prevCoin = -1;

		while (Sens_CoinEntry == 0)
		{
			if (prevCoin == -1)
			{
				uartWriteString("Zero\n");
				prevCoin = 0;
			}
		}

		uartWriteString("Coin Detected\n");
		prevCoin = 1;

		//printf("Detected input: %d \n", Sens_CoinEntry);
		//uartWriteString("Coin detected!");
		//char buffer [32];
		//sprintf(buffer,"Coin detected: %d",Sens_CoinEntry);
		//uartWriteString(buffer);

		while (Sens_CoinEntry != 0)
		{
			if (prevCoin == 1)
			{
				uartWriteString("Still detected!\n");
				prevCoin = -1;
			}
		}
	}
}

/* test_CoinDetectorDebounceTest
 *
 * Procedure used for testing that checks the coin detector for its ability to
 * detect coins after each other
 *
 */
void test_CoinDetectorDebounceTest(void)
{
	uart_printf("\nStart counting for 10 seconds... \n\n");
	waitFunction(10);    // wait 10 seconds
	uart_printf("board overview: \n");

	for (int i = 0; i < 7; i++)
		uart_printf("|%d", mem_Board[i]);

	printf("|\n");
}



void test_EndSwitchesInterrupt(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
			&& GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
			&& GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
	{
		puts(
				"power enabled, x axis enabled, z axis enabled\nwaiting for interrupt trigger...");
	}
	else
	{
		puts(
				"power not enabled, stopping test...\n\nplease reconfigure testing environment");
		while (1)
			;
	}
	while (1)
	{
		if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11))
		{    // motor power was disabled
			puts("motor power was disabled");
		}
		if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))
		{    // x axis was disabled
			puts("x axis was disabled");
		}
		if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
		{    // z axis was disabled
			puts("z axis was disabled");
		}
		if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
				&& !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
				&& !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
		{
			puts("all channels disabled\nending test");
			break;
		}
		if (!GPIO_ReadInputDataBit(HOME_END_PORT, HOMING_X))
		{
			DisableMotors(1, 1, 1);
		}
	}
}

void test_DifficultySwitchInput(void)
{
	printf("switch channel overview: \n    |ch1|ch2|ch3|ch4|\n\n");

	int valid = 0;
	int counter = 0;

	uint8_t input[4];
	uint8_t previous[4];

	while (1)
	{
		valid = 0;

		input[0] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
		input[1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
		input[2] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
		input[3] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);

		waitFunction(2);

		/* check if all values still the same, thus stabilized: */
		if (input[0] == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
				&& input[1] == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
				&& input[2] == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)
				&& input[3] == GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)) valid =
				1;

		if ((previous[0] != input[0] || previous[1] != input[1]
				|| previous[2] != input[2] || previous[3] != input[3]) && valid)
		{
			counter++;
			printf("%02d: |  %d|  %d|  %d|  %d|\n", counter, input[0], input[1],
					input[2], input[3]);
		}

		previous[0] = input[0];
		previous[1] = input[1];
		previous[2] = input[2];
		previous[3] = input[3];
	}
}

void test_EStopInput(void)
{
	int buttonnew = 0;
	int buttonold = 0;
	int valid = 0;

	puts("waiting for input...\n");

	while (1)
	{
		/* make sure the input is debounced */
		valid = 0;
		buttonnew = GPIO_ReadInputDataBit(E_STOP_PORT, E_STOP_PIN);
		waitFunction(2);
		if (buttonnew == GPIO_ReadInputDataBit(E_STOP_PORT, E_STOP_PIN)) valid =
				1;
		/* -------------------------------- */

		/* check if state changed and input is valid */
		if (buttonnew != buttonold && valid)
		{
			if (buttonnew) puts("Emergency stop input signal high (unpressed)");
			else puts("Emergency stop input signal low (pressed)");
		}
		buttonold = buttonnew;
	}
}

void test_ResetSwitchInput(void)
{
	int buttonnew = 0;
	int buttonold = 0;
	int valid = 0;

	puts("waiting for input...\n");

	while (1)
	{
		/* make sure the input is debounced */
		valid = 0;
		buttonnew = GPIO_ReadInputDataBit(RST_BUTTON_PORT, RST_BUTTON_PIN);
		waitFunction(2);
		if (buttonnew == GPIO_ReadInputDataBit(RST_BUTTON_PORT, RST_BUTTON_PIN)) valid =
				1;
		/* -------------------------------- */

		/* check if state changed and input is valid */
		if (buttonnew != buttonold && valid)
		{
			if (buttonnew) puts("Reset switch input signal high (unpressed)");
			else puts("Reset switch input signal low (pressed)");
		}
		buttonold = buttonnew;
	}
}

void test_RGBSensor(void)
{
	int inputnew = 0;
	sens = 0;
	uartWriteString("waiting for input...\n");

	while (1)
	{
		inputnew = readRGBSensor();

		if (inputnew == 1)
		{
			uartWriteString("Red coin\n");
		}
		else if (inputnew == 0)
		{
			uartWriteString("Yellow coin\n");
		}
		else
		{
			uartWriteString("No coin\n");
		}

		waitFunction(100);
	}
}

void test_Solenoid(void)
{
	while (1)
	{
		setSolenoidDutyCycle(60);
		waitFunction(40);
		setSolenoidDutyCycle(0);
		//TIM_SetCompare1(TIM15,0);
		waitFunction(70);
		/*int inputnew = readRGBSensor();

		if (inputnew == 1)
		{
			uartWriteString("Red coin");
		}

		else if (inputnew == 0)
		{
			uartWriteString("Yellow coin");
			setSolenoidDutyCycle(50);
			uartWriteString("open...\n");
			waitFunction(5);
			setSolenoidDutyCycle(0);
			uartWriteString("Dicht...\n");
		}

		else
		{
			uartWriteString("No coin");
		}

		waitFunction(100);*/
	}
}

void test_MotorX(void)
{

	EnableMotors(1, 1, 1);
	while (1)
	{
		moveToPos(000, 00, 1);
		waitFunction(100);
		moveToPos(100, 00, 1);
		waitFunction(100);
	}
}



void test_Servo(void)
{
	EnableMotors(1, 1, 1);

	waitFunction(300);
	setHorizontalVerticalServo(GRIPPER_RELEASE_ANGLE);

	uint16_t iteration = 0;

	while(1)
	{
		++iteration;
		uart_printf("\nIteration: %d\n", iteration);

		for(uint16_t n=1; n <= 7; n++)
		{

			uart_printf("Move to column: %d\n", n);
			uint16_t z = MZ_HOME + 10 + n * 16;
			moveToPos(mx_columnPos[n], z, 1.5);
			waitFunction(200);
		}
	}
}

void test_Slider(void)
{
	EnableMotors(0, 0, 1);
	waitFunction(500);

	for(int i = 0; i < 7; i++)
	{
		uart_printf("Release column: %d\n", i+1);
		setEmptyingServoPosition(i);
		waitFunction(500);
	}

	uart_printf("Close slider\n");
	CloseEmptyingServo();

	uart_printf("Slider test end!\n");
}

void test_Vacuum(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOA Periph clock enable */

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);

	/* Configure port A, pin 3 as output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InitStructure);

	while (1)
	{
		//GPIO_SetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);
		GPIO_SetBits(VAC_VALVE_PORT, VAC_VALVE_PIN);
		waitFunction(100);
		//GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);
		GPIO_ResetBits(VAC_VALVE_PORT, VAC_VALVE_PIN);
		waitFunction(100);
	}
}

int RandomNumberGenerator(const int nMin, const int nMax,
		const int nNumOfNumsToGenerate)
{
	int nRandonNumber = 0;
	for (int i = 0; i < nNumOfNumsToGenerate; i++)
	{
		nRandonNumber = rand() % (nMax - nMin) + nMin;

	}
	return nRandonNumber;
}

void test_endurance(void)
{
	StackChoice = whatStackToEmpty();
	uart_printf("Picking up coin from stack: %d", StackChoice);

	moveToPos(mx_storePos[StackChoice], MZ_STORE + 60, 2);
	// rotate and lower to storage
	setHorizontalVerticalServo(SERVO_VERT);
	waitFunction(50);

	waitFunction(50);

	int column = 1;
	int i = 1;
	int k = 8;
	int max = 0;
	int max_mem = 2;
	int vertical = 0;
	uint8_t iteration = 0;

	uint8_t coinsInStack = 0;

	while (1)
	{
		uart_printf("Iteration: %d", iteration);

		while (max < max_mem)
		{

			while (i < k && max < max_mem && vertical == 0)
			{
				play_ActionCompleted = 0;
				uart_printf("Play coin in column: %d", column);
				coinsInStack = standardPlayLoop(column, coinsInStack);
				column = ++i;
			}

			while (i < k && max < max_mem && vertical == 1)
			{
				play_ActionCompleted = 0;
				i++;
				uart_printf("Play coin in column: %d", column);
				coinsInStack = standardPlayLoop(column, coinsInStack);

				if (i == 7)
				{
					column++;
					max++;
					i = 1;
				}

			}
			i = 1;
			column = i;
			max++;
			if (vertical == 1)
			{
				column = max;
			}
		}
		uart_printf("End game");
		performEndOfGameLogic();

		rood = 0;
		max = 1;
		i = 1;
		column = 1;

		mem_StorageStack[0] = 7;	// storage, starts filled during game start!
		mem_StorageStack[1] = 7;	// storage, starts filled during game start!
		mem_StorageStack[2] = 7;	// storage, starts filled during game start!
		if (max_mem < 4)
		{
			max_mem++;
		}
		if (max_mem >= 4 && vertical == 0)
		{
			max_mem = 2;
			vertical = 1;
		}
		if (max_mem >= 4 && vertical == 1)
		{
			max_mem = 2;
			vertical = 0;
		}
	}
}

