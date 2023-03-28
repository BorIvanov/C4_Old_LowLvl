/*
 * test_tunePIDs.c
 *
 *  Created on: 2 Mar 2019
 *      Author: ashri
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <stdlib.h>
#include "vars.h"
#include "uart.h"

#include "servoControl.h"
#include "motorpwm.h"
#include "motorControlInterrupt.h"
#include "motorPIDs.h"

#include "test_tunePIDs.h"



enum testFunctions{
	TUNE_ZPIDS,
	TUNE_ZPID_DOWN,
	ZSTAIRS,
	ZSTAIRS_DOWN,
	ZSTAIRS_BOTH,
	ZSTAIRS_LONG,
	ZSTAIRS_LONG_DOWN,
	BENCHMARK,
	BENCHZ,
	RANDOM
};

typedef struct
{
	double Kp;
	double Ki;
	double Kd;
	double MaxSpeed;
}testPID;


/*
 * Main function
 *
 * Parameters:
 * testParam => currently does nothing, but can be used to select 'z' or 'x' tests
 */
void test_PIDsMain(const char *testParam)
{
	EnableMotors(1, 1, 1); 	// the z-motor should always be enabled
	enableWriteString = 1;

	enum testFunctions toTest = BENCHZ;

	switch(toTest)
	{
		// test z-axis
		case TUNE_ZPIDS:
		{
			uart_printf("test_TuneZPIDs\n");
			test_tuneZPIDs("up");
			break;
		}
		case TUNE_ZPID_DOWN:
		{
			uart_printf("test_TuneZPIDs (down)\n");
			test_tuneZPIDs("down");
			break;
		}
		case ZSTAIRS:
		{
			uart_printf("testZStars\n");
			test_zStairs("up");
			break;
		}
		case ZSTAIRS_DOWN:
		{
			uart_printf("testZStairs (down)\n");
			test_zStairs("down");
			break;
		}
		case ZSTAIRS_BOTH:
		{
			uart_printf("testZStairs (down)\n");
			test_zStairs("both");
			break;
		}
		case ZSTAIRS_LONG:
		{
			uart_printf("testZStairs (long)\n");
			test_zStairs("long");
			break;
		}
		case ZSTAIRS_LONG_DOWN:
		{
			uart_printf("testZStairs (long)\n");
			test_zStairs("long_down");
			break;
		}
		case BENCHMARK:
		{
			uart_printf("test_benchmark()\n");
			test_benchmark();
			break;
		}
		case BENCHZ:
		{
			uart_printf("void test_benchZ()\n");
			test_benchZ();
			break;
		}
		case RANDOM:
		{
			uart_printf("test Random\n");
			test_moveToPosXY(MX_FLIPPER, MZ_STORE);
			break;
		}
	}

	enableWriteString = 1;
	uart_printf("\n/-----------------------------/\n");


	while(1)
	{
		// do nothing
	}
}


/*
 * Sets PID values
 */
void test_setPIDs(double Kp, double Ki, double Kd, double max_speed)
{
	 mz_positionPID_Kp = Kp;			// proportional PID component (was 0.008)
	 mz_positionPID_Ki = Ki;			// (was 0.016) note that the I control does have a dump if passing through the setpoint.
	 mz_positionPID_Kd= Kd;				// (was 0.0008) differential PID component, positive is real damping, negative is destabilation
	 mz_MaxMotorSpeed = max_speed;		// maximum permissible motor speed in rpm@motor shaft
	 mz_PIDIntCnt = 0;					// reset integrator
}


/*
 * the idea here was to swap between an iterative an binary search
 */
double test_iterSearch(double start, double end, int max, int current)
{
	double retVal = start + current * ((end - start)/max);
	return retVal;
}

/*
 * unfortunately, the binary search never got implemented
 */
//double test_binSearch(double start, double end, )


/*
 * tests different PID values
 */
void test_tuneZPIDs(const char* direction)
{
	testPID pid_compare, pid_home;
	int32_t startZ, endZ;

	if(strcmp(direction, "down") == 0)
	{
		// test downwards PIDs
		startZ = MZ_TOP;
		//endZ = MZ_STORE - MZ_CHIP_HEIGHT * (8 - 3);
		endZ = 0;

		pid_compare.Kp = 0.008;
		pid_compare.Ki = 0;
		pid_compare.Kd = 0.001;
		pid_compare.MaxSpeed = 100;

		pid_home.Kp = 0.018*2;
		pid_home.Ki = 0.025*3;
		pid_home.Kd = 0;
		pid_home.MaxSpeed = 1000;
	}
	else
	{
		// upwards PIDs
		startZ = 0;
		endZ = 50;

		pid_compare.Kp = 0.01;
		pid_compare.Ki = 0;
		pid_compare.Kd = 0;
		pid_compare.MaxSpeed = 1000;

		pid_home.Kp = 0.0065;
		pid_home.Ki = 0.01;
		pid_home.Kd = 0.00;
		pid_home.MaxSpeed = 1000;
	}

	// if starting potision is at top => move to top
	if(startZ > endZ)
	{
		enableWriteString = 0;
		waitFunction(100);
		test_setPIDs(pid_home.Kp, pid_home.Ki, pid_home.Kd, pid_home.MaxSpeed);
		test_moveToZPosTune(startZ);
	}

	waitFunction(100);

	// Notes: upwareds
	// PID = {0.01, 0.02, 0} gets all setpoints except 0-50 => choose PID = {0.01, 0.03, 0}


	// Notes: downwards
	// P = 0.002 => 64; 0.004 => 52; 0.006 => 48.25 ==> does not work with stairs => see logs/2019-03-06_140735.csv
	// Chosen: PID = (0.008, 0.01, 0) => chosen (0.0065, 0.01, 0)
	// tune range: P; I; D => (0.002, 0.02, 8); (0, 0.02, 4); (0, 0, 1)
	double Kp_start = 0.002;
	double Kp_end = 0.01;
	int Kp_steps = 8;

	double Ki_start = 0;
	double Ki_end = 0.02;
	double Ki_steps = 4;

	double Kd_start = pid_compare.Kd;
	double Kd_end = 0;
	int Kd_steps = 1;

	double Kp = 0;
	double Ki = 0;
	double Kd = 0;


	for(int id = 0; id < Kd_steps; id++)
	{
		//double Kd = Kd_start + id * ((Kd_end - Kd_start)/Kd_steps);
		Kd = test_iterSearch(Kd_start, Kd_end, Kd_steps, id);
		for(int ii = 0; ii < Ki_steps; ii++)
		{
			//double Ki = Ki_start + ii * ((Ki_end - Ki_start)/Ki_steps);
			Ki = test_iterSearch(Ki_start, Ki_end, Ki_steps, ii);
			for(int ip = 0; ip < Kp_steps; ip++)
			{
				//double Kp = Kp_start + ip * ((Kp_end - Kp_start)/Kp_steps);
				Kp = test_iterSearch(Kp_start, Kp_end, Kp_steps, ip);
				pid_compare.Kp = Kp;
				pid_compare.Ki = Ki;
				pid_compare.Kd = Kd;
				enableWriteString = 1;
				waitFunction(100);
				test_setPIDs(pid_compare.Kp, pid_compare.Ki, pid_compare.Kd, pid_compare.MaxSpeed);
				test_moveToZPosTune(endZ);

				// move to beginning
				enableWriteString = 0;
				waitFunction(100);
				test_setPIDs(pid_home.Kp, pid_home.Ki, pid_home.Kd, pid_home.MaxSpeed);
				test_moveToZPosTune(startZ);
			}
		}
	}

	// move as low as possible to ease 0-calibaration
	if(startZ > endZ)
	{
		enableWriteString = 0;
		waitFunction(100);
		test_setPIDs(pid_compare.Kp, pid_compare.Ki, pid_compare.Kd, pid_compare.MaxSpeed);
		test_moveToZPosTune(endZ);
	}

	enableWriteString = 1;
}


/*
 * use a binary search method to determine PIDs
 */

// x > y => 1
// x = y => 0
// x < y => -1
int test_abscompare(double x, double y)
{
    double a = x >= 0 ? x : 0 - x;
    double b = y >= 0 ? y : 0 - y;

    // abs and labs were acting odd and resulted in an odd convergence point
    if(a > b) return 1;
    else if(a < b) return -1;
    else return 0;
}

double test_binfunc(int targetX, int startZ, int targetZ)
{
    test_moveToPosXY(targetX, startZ);
    waitFunction(100);
    test_moveToPosXY(targetX, targetZ);

    return targetZ - (mx_FullPos / Mem_PositionConstant);
}

double test_binsearch(double *min, double *mid, double *max, double *fmin, double *fmid, double *fmax)
{
    if(test_abscompare(*fmid, *fmax) == -1 && test_abscompare(*fmax, *fmin) == 1)
    {
        *max = *mid;
        *fmax = *fmid;
    }
    else
    {
        *min = *mid;
        *fmin = *fmid;
    }
    return (*min + ((*max) - (*min))/ 2);
}

void test_binsearchMethod()
{
//	double Kp_min = 0.003;
//	double Kp_max = 0.01;
//	double Kp_mid = Kp_min + (Kp_max - Kp_min)/2;
//	double Kp_fmin, Kp_fmid, Kp_fmax;
//
//	double Ki_min = 0.001;
//	double Ki_max = 0.015;
//	double Ki_mid = Ki_min + (Ki_max - Ki_min)/2;
//	double Ki_fmin, Ki_fmid, Ki_fmax;


}

/*
 * test chosen PID values
 */
void test_zStairs(const char* direction)
{
	uint16_t startZ = 0;
	uart_printf("testing stair function\n\n");

	//double pid_up[] = {0.018*2, 0.025*4, 0, 1000};
	//double pid_down[] = {0.0065*2, 0.01, 0.00, 1000};
	double pid_up[] = {0.018*2, 0.025*3, 0, 1000};
	//double pid_down[] = {0.0065, 0.01, 0.00, 1000};
	//double pid_down[] = {0.077, 0.01, 0.00, 500};
	double pid_down[] = {0.077, 0.005, 0.004, 500};

	uint8_t printUp = 1;
	uint8_t printDown = 1;

	if(strcmp(direction, "down") == 0) printUp = 0;
	if(strcmp(direction, "up") == 0) printDown = 0;
	if(strcmp(direction, "both") == 0)
	{
		// print both
	}


	if(strcmp(direction, "long") == 0)
	{
		for(int i=10; i<=350; i=i+10)
		{
			enableWriteString = printUp;
			waitFunction(100);
			test_setPIDs(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
			test_moveToZPosTune(i);
		}
		enableWriteString = 0;
		test_setPIDs(pid_down[0], pid_down[1], pid_down[2], pid_down[3]);
		test_moveToZPosTune(startZ);
	}
	else if(strcmp(direction, "long_down") == 0)
	{
		enableWriteString = 0;
		test_setPIDs(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
		test_moveToZPosTune(350);

		for(int i=350; i>=0; i=i-10)
		{
			enableWriteString = printDown;
			waitFunction(100);
			test_setPIDs(pid_down[0], pid_down[1], pid_down[2], pid_down[3]);
			test_moveToZPosTune(i);
		}
	}
	else
	{
		for(int i=50; i<=300; i=i+50)
		{
			enableWriteString = printUp;
			test_setPIDs(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
			test_moveToZPosTune(i);

			// move to 0
			enableWriteString = printDown;
			test_setPIDs(pid_down[0], pid_down[1], pid_down[2], pid_down[3]);
			test_moveToZPosTune(startZ);
		}
	}

	enableWriteString = 1;
	uart_printf("Done with steps");
}



/*
 * MoveToZPos helper function
 */
void test_moveToZPosTune(int16_t targetZ)
{
	test_moveToZPos(targetZ, 1);
	//test_moveToZPosNoWait(targetZ);
}


void test_moveToZPosNoWait(int16_t targetZ)

{
	EnableMotors(1, 1, 1);
	mz_ismoving = calcDirection(mz_FullPos, targetZ * Mem_PositionConstant);

	// print stuff
	uart_printf("\nMove to: %d in test_moveToZPosNoWait\n", targetZ);
	if(mz_ismoving == 1)
	{
		uart_printf("|-----------------------\nNew z-position\n\nMovement: Up\n");
	}
	else
	{
		uart_printf("|-----------------------\nNew z-position\n\nMovement: Down\n");
	}
	// Pass selected PIDs to UART
	uart_printf("P,\t %.4f\nI,\t %.4f\nD,\t %.4f\nPosConst,\t %.2f\n\n", mz_positionPID_Kp, mz_positionPID_Ki, mz_positionPID_Kd, Mem_PositionConstant);
	uart_printf("\tTime, \t Setpoint,\t Position, Delta (mm)\n");
//	uart_printf("    0.00,\t %8.2f,\t %8.2f,\t %8.2f\n",
//				(double)mz_PosTarget/Mem_PositionConstant, (double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant);

	// set target pos
	uint16_t nBounceCount = 0;
	uint32_t timeCount = 0;
	mz_PosTarget = targetZ * Mem_PositionConstant;
	TIM7_CustomTimerCounter = 0; // start @ 0 sec

	// Wait until position attained
	while(1)
	{
		uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
						(double)timeCount/10, (double)mz_PosTarget/Mem_PositionConstant,
						(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant);
		waitFunction(10);
		timeCount++;
		if(labs(mz_FullPos - mz_PosTarget) < 1.5 * Mem_PositionConstant)
		{
			nBounceCount++;
			if(nBounceCount > 2)
			{
				break;
			}
		}
		else
		{
			nBounceCount = 0;
		}
	}

	uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
				(double)timeCount/10, (double)mz_PosTarget/Mem_PositionConstant,
				(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant);
	uart_printf("-----------------------|\n");
	uart_printf("nBounce: %d in %d\n", nBounceCount, timeCount);

	mz_ismoving = 0;

}



void test_selectZPID(int16_t targetX, int16_t targetZ)
{
	//double currentZ = mz_FullPos/Mem_PositionConstant;

	// z constants

	// don't do anything if moving upwards
	if(mz_ismoving == 1)return;

//	 mz_storePos[0]: -4
//	 mz_storePos[1]:  3
//	 mz_storePos[2]: 10
//	 mz_storePos[3]: 17
//	 mz_storePos[4]: 24
//	 mz_storePos[5]: 31
//	 mz_storePos[6]: 38
//	 mz_dumpPos[0]: -42
//	 mz_dumpPos[1]: -35
//	 mz_dumpPos[2]: -28
//	 mz_dumpPos[3]: -21
//	 mz_dumpPos[4]: -14
//	 mz_dumpPos[5]:  -7

	// otherwise check state
	if(targetZ >= MZ_DROP) // drop coin in matrix
	{
		test_setPIDs(0.077, 0.005, 0.001, 500);
	}
	else if(targetZ >= MZ_DUMP_STACK)
	{
		test_setPIDs(0.007, 0.005, 0.002, 500);
	}
	else if(targetZ >= mz_storePos[6]) // top stack coin
	{
		test_setPIDs(0.002, 0.0015, 0.002, 500);
	}
	else if(targetZ >= mz_storePos[5]) // stack coin #6
	{
		test_setPIDs(0.003, 0.0015, 0.002, 500);
	}
	else if(targetZ >= mz_storePos[4]) // stack coin #5
	{
		test_setPIDs(0.005, 0.0015, 0.002, 500);
	}
	else if(targetZ >= mz_storePos[3]) // stack coin #4
	{
		test_setPIDs(0.005, 0.0015, 0.002, 500);
	}
	else if(targetZ >= mz_storePos[0]) // stack coin #1-3
	{
		test_setPIDs(0.007, 0.005, 0.002, 500);
	}
	else if(targetZ >= mz_dumpPos[4]) // dump coin #5-6
	{
		test_setPIDs(0.007, 0.010, 0.002, 500);
	}
	else if(targetZ >= mz_dumpPos[3]) // dump coin #4
	{
		test_setPIDs(0.007, 0.010, 0.002, 500);
	}
	else if(targetZ >= mz_dumpPos[1]) // dump coin #2-3
	{
		test_setPIDs(0.008, 0.015, 0.002, 500);
	}
	else if(targetZ >= mz_dumpPos[0]) // dump coin #1
	{
		test_setPIDs(0.007, 0.015, 0.001, 500);
	}
	else
	{
		test_setPIDs(0.007, 0.015, 0.001, 500);
	}

//	else
//	{
//
//		if(targetZ == mz_storePos[0] ||
//		   targetZ == mz_storePos[1] ||
//		   targetZ == mz_storePos[2] )
//		{
//			test_setPIDs(0.007, 0.005, 0.002, 500);
//		}
//		else if(targetZ == mz_storePos[3] ||
//				targetZ == mz_storePos[4])
//		{
//			test_setPIDs(0.005, 0.0015, 0.002, 500);
//		}
//		else if(targetZ == mz_storePos[5] ||
//				targetZ == mz_storePos[6])
//		{
//			test_setPIDs(0.003, 0.0015, 0.002, 500);
//		}
//		else if(targetZ == mz_dumpPos[0] ||
//				targetZ == mz_dumpPos[1] ||
//				targetZ == mz_dumpPos[2] ||
//				targetZ == mz_dumpPos[3] ||
//				targetZ == mz_dumpPos[4] ||
//				targetZ == mz_dumpPos[5])
//		{
//			test_setPIDs(0.007, 0.005, 0.002, 500);
//		}
//		else
//		{
//			test_setPIDs(0.007, 0.005, 0.002, 500);
//		}
//	}

	/*
	else if(targetZ < 200 && currentZ > MZ_DROP) // go from MZ_TOP -> coin storage
	{
		if(targetZ > 25)
		{
			test_setPIDs(0.003, 0.0015, 0.002, 500);
		}
		else if(targetZ > 15 && targetZ <= 25) // top-most coins
		{
			//test_setPIDs(0.077, 0.005, 0.001, 500);
			//test_setPIDs(0.003, 0.005, 0.0025, 500);
			test_setPIDs(0.005, 0.002, 0.002, 500);
		}
		else
		{
			test_setPIDs(0.007, 0.005, 0.002, 500);
		}
	}
	else // going from middle =>
	{
		//test_setPIDs(0.008, 0.015, 0.001, 1000);
		//test_setPIDs(0.008, 0.015, 0.001, 500);
		test_setPIDs(0.07, 0.025, 0.001, 500);
	}*/
}

/*
 * move to x-y
 */
void test_moveToPosXY(int16_t targetX, int16_t targetZ)
{
        EnableMotors(1, 1, 1);
        mx_ismoving = calcDirection(mx_FullPos, targetX * Mem_PositionConstant);
        mz_ismoving = calcDirection(mz_FullPos, targetZ * Mem_PositionConstant);


        // ================================================
        // downwards movement => move x-axis first
        if(mz_ismoving == -1)
        {
        	uart_printf("Moving x-axis\n");
        	mx_PosTarget = targetX * Mem_PositionConstant;
            while(labs(mx_FullPos - mx_PosTarget) > 1.5 * Mem_PositionConstant){
            	// do nothing
            }
        }

        // ================================================
        // set the proper PIDs
    	double pid_up[] = {0.018*2, 0.025*3, 0, 1000};
    	//double pid_down[] = {0.0065, 0.01, 0.00, 1000};
    	//double pid_down[] = {0.0075, 0.01, 0.001, 100};
    	// double pid_down[] = {0.002, 0.006, 0, 100};
    	//double pid_down[] = {0.077, 0.005, 0.004, 500};
    	//double pid_down[] = {0.008, 0.015, 0.001, 100};

    	if(mz_ismoving == 1)
    	{
    		uart_printf("|-----------------------\nNew z-position\n\nMovement: Up\n");
    		test_setPIDs(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);
    	}
    	else
    	{
    		uart_printf("|-----------------------\nNew z-position\n\nMovement: Down\n");
    		//test_setPIDs(pid_down[0], pid_down[1], pid_down[2], pid_down[3]);
    		test_selectZPID(targetX, targetZ);
    	}

        // ================================================
        // move z-axis
    	uart_printf("P,\t %.4f\nI,\t %.4f\nD,\t %.4f\nPosConst,\t %.2f\n\n", mz_positionPID_Kp, mz_positionPID_Ki, mz_positionPID_Kd, Mem_PositionConstant);
    	uart_printf("\tTime, \t Setpoint,\t Position, Delta (mm)\n");

    	uint16_t nBounceCount = 0;
    	uint32_t timeCount = 0;
        mz_PosTarget = targetZ * Mem_PositionConstant;

    	while(1)
    	{
    		uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
    						(double)timeCount/7, (double)mz_PosTarget/Mem_PositionConstant, // note: 7 -> sould be 10, but because of delays is closer to 7
    						(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant);
    		waitFunction(10);
    		timeCount++;
    		if(labs(mz_FullPos - mz_PosTarget) < 1.5 * Mem_PositionConstant)
    		{
    			nBounceCount++;
    			if(nBounceCount >= 2)
    			{
    				break;
    			}
    		}
    		else if(timeCount > 35)
    		{
    			break;
    		}
    		else
    		{
    			nBounceCount = 0;
    		}
    	}

    	uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
    				(double)timeCount/7, (double)mz_PosTarget/Mem_PositionConstant,
    				(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant);
    	uart_printf("-----------------------|\n");

    	test_setPIDs(pid_up[0], pid_up[1], pid_up[2], pid_up[3]);

    	// ================================================
        // upwards movement => move x-axis after z-axis completed movement
        if(mz_ismoving == 1)
        {
        	uart_printf("Moving x-axis\n");
        	mx_PosTarget = targetX * Mem_PositionConstant;
            while(labs(mx_FullPos - mx_PosTarget) > 1.5 * Mem_PositionConstant){

            }
        }


        mx_ismoving = 0;
        mz_ismoving = 0;
}


void test_benchZ()
{
	uint8_t printUp = 0;
	uint8_t printDown = 1;
	waitFunction(200);

	for(int i = 6; i >= 0; i--)
	{
		uart_printf("mz_storePos[%d]: %lf\n", i, mz_storePos[i]);
	}
	for(int i = 5; i >= 0; i--)
	{
		uart_printf("mz_dumpPos[%d]: %lf\n", i, mz_dumpPos[i]);
	}
	//return;

	// check coin drop
	setHorizontalVerticalServo(SERVO_HORZ);
	for(int column = 1; column <=1; column++)
	{
		uint16_t zpos[] = {
				MZ_TOP,
				MZ_DROP
		};
		enableWriteString = 1;
		uart_printf("Coin drop; column: %d\n", column);

		for(int z = 0; z<sizeof(zpos)/sizeof(zpos[0]); z++)
		{
			uint16_t z_old = z==0 ? MZ_HOME : zpos[z-1];
			if(z_old < zpos[z]) enableWriteString = printUp;
			else enableWriteString = printDown;

			waitFunction(100);
			test_moveToPosXY(mx_columnPos[column], zpos[z]);
		}
	}

	// move to middle
	waitFunction(100);
	test_moveToPosXY(0,  MZ_DUMP_STACK + 60);
	setHorizontalVerticalServo(SERVO_HORZ);
	waitFunction(100);


	// check top => coin stack
	for(int coin = 7; coin > 0; coin--)
	{
		// magic number to coin location in storage stack
		//uint16_t z_coin = MZ_STORE - (8 - coin) * MZ_CHIP_HEIGHT;
		double z_coin = mz_storePos[coin-1];

		enableWriteString = 1;
		uart_printf("stack; coin: %d @ %lf\n", coin, z_coin);

		enableWriteString = printUp;
		waitFunction(100);
		test_moveToPosXY(MX_HOME, MZ_TOP);

		enableWriteString = printDown;
		waitFunction(100);
		test_moveToPosXY(MX_HOME, z_coin);
	}

	// adjust servo
	waitFunction(100);
	enableWriteString = printUp;
	test_moveToPosXY(mx_FullPos / Mem_PositionConstant, MZ_DUMP_STACK + 60);
	setHorizontalVerticalServo(SERVO_VERT - 2);

	// grab from dumpstack
	for(int coin = 6; coin > 0; coin--)
	{
		//uint16_t z_coin = MZ_DUMP_STACK - 45 - (8 - coin) * MZ_CHIP_HEIGHT;
		double z_coin = mz_dumpPos[coin-1];

		enableWriteString = 1;
		uart_printf("Dump; coin: %d @ %lf\n", coin, z_coin);

		enableWriteString = printUp;
		waitFunction(100);
		//test_moveToPosXY(MX_DUMP_STACK, MZ_STORE+5);
		test_moveToPosXY(MX_DUMP_STACK, MZ_DUMP_STACK);

		enableWriteString = printDown;
		waitFunction(100);
		test_moveToPosXY(MX_DUMP_STACK, z_coin);
	}

	// go back to home
	enableWriteString = 0;
	waitFunction(100);
	test_moveToPosXY(mx_FullPos / Mem_PositionConstant, MZ_DUMP_STACK + 60);
	setHorizontalVerticalServo(SERVO_HORZ);
	waitFunction(100);
	test_moveToPosXY(MX_HOME, MZ_HOME);
}

/*
 * benchmark a more accurate x-z movement
 */

void test_benchmark()
{
	// set variables
	uint16_t test_StackChoice = 0;
	uint16_t test_columnNr = 1;

	for(int coin = 7; coin > 0; coin--)
	{
		for(int state = 2; state <= 10; state++)
		{
			uart_printf("Next movement: %d\n", state);
			waitFunction(100);
			switch(state)
			{
				case 1:
				{
					// do nothing
					break;
				}
				case 2:
				{
					// move to stack height
					uint16_t x = mx_storePos[test_StackChoice];
					uint16_t z = MZ_STORE - MZ_CHIP_HEIGHT * (8 - coin);
					test_moveToPosXY(x, z);
					break;
				}
				case 3:
				{
					// enable vacuum
					break;
				}
				case 4:
				{
					// move to top of board
					uint16_t x = mx_columnPos[test_columnNr];
					uint16_t z = MZ_TOP;
					test_moveToPosXY(x, z);
					break;
				}
				case 5:
				{
					// rotate gripper horizontal
					//setHorizontalVerticalServo(SERVO_HORZ);
					break;
				}
				case 6:
				{
					// move to top
					uint16_t x = mx_columnPos[test_columnNr];
					uint16_t z = MZ_DROP;
					//test_moveToPosXY(x, z);
					break;
				}
				case 7:
				{
					// disable vacuum
					break;
				}
				case 8:
				{
					// move back to top of board => identical to state 4
					uint16_t x = mx_columnPos[test_columnNr];
					uint16_t z = MZ_TOP;
					//test_moveToPosXY(x, z);
					break;
				}
				case 9:
				{
					// rotate gripper vertical
					//setHorizontalVerticalServo(SERVO_VERT);
					break;
				}
				case 10:
				{
					// move back to stack??
					uint16_t x = mx_storePos[test_StackChoice];
					uint16_t z = MZ_STORE - MZ_CHIP_HEIGHT * (8 - mem_StorageStack[test_StackChoice]);
					//test_moveToPosXY(x, z);
					break;
				}
			} // end switch
		} // end loop
	} // end coin test
} // end test_benchmark


/*
 * Copied moveToPos here to allow for tests/modifications without effecting original implementation
 *
 * Parameters:
 * targetZ => setpoint for z-position
 */
void test_moveToZPos(int16_t targetZ, uint8_t fixedInterfal)
{
	// announce movement
	uart_printf("\nMove to: %d\n", targetZ);
	mz_ismoving = calcDirection(mz_FullPos, targetZ * Mem_PositionConstant);

	// Separate PIDs for UP/Down
	if(mz_ismoving == 1)
	{
		uart_printf("|-----------------------\nNew z-position\n\nMovement: Up\n");
	}
	else
	{
		uart_printf("|-----------------------\nNew z-position\n\nMovement: Down\n");
	}


	// Pass selected PIDs to UART
	uart_printf("P,\t %.4f\nI,\t %.4f\nD,\t %.4f\nPosConst,\t %.2f\n\n",
			mz_positionPID_Kp, mz_positionPID_Ki, mz_positionPID_Kd, Mem_PositionConstant);
	uart_printf("\tTime, \t Setpoint,\t Position, Delta (mm)\n");

	// start movement
	int32_t currPos = 0;
	TIM7_CustomTimerCounter = 0; // start @ 0 sec

	mz_PosTarget = targetZ * Mem_PositionConstant;
	//mz_ismoving = calcDirection(mz_FullPos, targetZ * Mem_PositionConstant);

	//while (labs(mz_FullPos - mz_PosTarget) > 1.5 * Mem_PositionConstant)
	while(1)
	{
		if(currPos != mz_FullPos)
		{
			uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
					(double)TIM7_CustomTimerCounter/100, (double)mz_PosTarget/Mem_PositionConstant,
					(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant );
			currPos = mz_FullPos;
		}

		if(fixedInterfal == 1)
		{
			if(TIM7_CustomTimerCounter >= 500)
			{
				// exit the loop if the setpoint has not been reached in sufficient time
				break;
			}
		}
		else
		{
			if(labs(mz_FullPos - mz_PosTarget) < 1.5 * Mem_PositionConstant)
			{
				// exit loop if within range
				break;
			}
		}
	} // end while

	// Prit final values
	uart_printf("%8.2f,\t %8.2f,\t %8.2f,\t %8.2f\n",
			(double)TIM7_CustomTimerCounter/100, (double)mz_PosTarget/Mem_PositionConstant,
			(double)mz_FullPos/Mem_PositionConstant, (mz_PosTarget-mz_FullPos)/Mem_PositionConstant );

	uart_printf("-----------------------|\n");

	// restore old PIDs
	mz_positionPID_Kp = 0.018;			// proportional PID component (was 0.008)
	mz_positionPID_Ki = 0.025;			// (was 0.016) note that the I control does have a dump if passing through the setpoint.
	mz_positionPID_Kd= 0.0;			// (was 0.0008) differential PID component, positive is real damping, negative is destabilation
	mz_MaxMotorSpeed = 1000;			// maximum permissible motor speed in rpm@motor shaft


	mx_ismoving = 0;
	mz_ismoving = 0;
}


/*
 * simplifies sending text to RPI
 */
void uart_printf(char *format, ...)
{
	if(enableWriteString)
	{
		char szPrint[1024];

		va_list args;
		va_start(args, format);
		vsnprintf(szPrint, sizeof(szPrint), format, args);
		va_end(args);

		uartWriteString(szPrint);
	}
}

