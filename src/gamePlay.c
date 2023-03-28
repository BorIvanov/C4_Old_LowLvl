/*
 * gamePlay.c
 *
 *  Created on: 25 Sep 2017
 *      Author: lboerefijn
 */

#include <solenoidControl.h>
#include "vars.h"
#include "uart.h"
#include "gamePlay.h"
#include "motorPIDs.h"
#include "servoControl.h"
#include "motorControlInterrupt.h"
#include "rgb_light_control.h"
#include "readDifficulty.h"
#include "checkForCoinPlay.h"
#include "motorpwm.h"

#define UART_ACK_DELAY (uint8_t)100000
#define MAX_DELTA_X 1.5 // mm precision
#define MAX_DELTA_Z 1.5 // mm precision

/* HELP FUNCTIONS */

/* HANDLING RASPBERRY PI COMMANDS */
/**
 * initGamePlay: acknowledge to PI that bootup and homing is complete
 */
void initGamePlay()
{
    GPIO_SetBits(DLED_PORT, DLEDS);
    StackChoice = whatStackToEmpty();

    GPIO_ResetBits(DLED_PORT, DLED1);
    moveToPos(MX_HOME, MZ_STORE + 20, 2);
    waitFunction(20);
    setHorizontalVerticalServo(SERVO_VERT);
    GPIO_ResetBits(DLED_PORT, DLED2);
    waitFunction(20);

    CloseEmptyingServo();
    GPIO_ResetBits(DLED_PORT, DLED3);
    waitFunction(20);
    moveToPos(mx_storePos[StackChoice], MZ_STORE + 10, 2);
    GPIO_ResetBits(DLED_PORT, DLED4);
    waitFunction(20);

    if (ackCmdValid())
    {
    	state = CMD_INIT;
    }
}

/**
 * startGamePlay: prepare for start new game
 *
 * Check if end of game logic is finished
 */
void startGamePlay()
{
    // Only allowed if we came from the state Init or GameEnd:
    if (state != CMD_INIT && state != CMD_GAME_END)
        return;

    if (ackCmdValid())
    {

        //TODO check if ready to start game
    	state = CMD_START;
    }
}
/**
 * selectDifficulty: choose difficulty with dial
 */
void selectDifficulty()
{
    // Only allowed if we came from the state start:
    if (state != CMD_START)
        return;

    if (ackCmdValid())
    {
        unsigned char difficulty;

        difficulty = 0x02; //readDifficulty(); // read difficulty

        uartWriteCommandWithAck(CMD_DIFFICULTY, difficulty, UART_ACK_DELAY);

        state = CMD_DIFFICULTY;
    }
}
/**
 * detectHumanInput: detect human move
 *
 * Use the light gates to detect human input.
 * Communicate column to the PI
 * Also check for cheats (optionally elsewhere?)
 */
void detectHumanInput() // select a column to play for the humanoid, totally not a robot, player
{
    // Only allowed if we came from the state difficulty or insert coin:
    if (state != CMD_DIFFICULTY && state != CMD_INSERT_COIN)
        return;

    if (ackCmdValid())
    {
        uint8_t columnNr = 0; // initiate columnNr

        //GPIO_ResetBits(GPIOE, LEDS);

        while (checkcoin() == -1)
        {
        } //wait for coin
        waitFunction(15);
        columnNr = checkcoin();

        // if more than one coin is dropped (columnNr == 9)
        // or if two coins on the same time are dropped (detect_error != 0)
        // this is cheating and the game is ended
        // Needs another approach in new object oriented architecture!!
        if (columnNr == 9 || detect_error != 0)
        {
            performEndOfGameLogic();
        	detect_error = 0;
            state = CMD_GAME_END;
        }
        if (checkcoin() != 9)
        {
            for (int k = 0; k < 7; k++)
            {
                mem_Board_old[k] = mem_Board[k];
            } //update board
        }

        // logic to handle communication to rPI
        columnNr--; // rPI handles column no 0 to 6, not 1 to 7
        uartWriteCommandWithAck(CMD_COLUMN, columnNr, UART_ACK_DELAY);

        //addToColumn(columnNr);

        //GPIO_ResetBits(GPIOE, LEDS);
        state = CMD_COLUMN;
    }
}

/**
 * insertCoin: pick up and insert coin
 *
 * Place a coin in the column determined by the PI
 */
void insertCoin()
{
    // Only allowed if we came from the state column
    if (state != CMD_COLUMN)
        return;

    if (ackCmdValid())
    {
        uint8_t column = uartPacket[2];
        int returnval = -1;
        column++;

        while (returnval != 0)
        {
            if (returnval == -1)
            {
                returnval = 0;
            }
            returnval = standardPlayLoop(column, returnval);
        }

        //addToColumn(column);

        //TODO check for cheats
        column--;
        uartWriteCommandWithAck(CMD_INSERT_COIN, column, UART_ACK_DELAY);

        state = CMD_INSERT_COIN;
    }
}
/**
 * gameEnd: end of game logic
 *
 * Clear board and sort the yellow/red chips.
 */
void gameEnd() // The command handling to empty the board
{
    // Only allowed if we came from the state column or coin
    if (state != CMD_COLUMN && state != CMD_INSERT_COIN)
        return;

    if (ackCmdValid())
    {
        /* TODO celebrate finishing with a little dance */

        performEndOfGameLogic();
        //DisableMotors();
        state = CMD_GAME_END;
    }
}

// big functions

/**
 * standardPlayLoop: logic to place chip in specified column
 *
 * Controls motors, rotational servo and vacuum to pick up and place a chip
 *
 * Parameters:
 * 	columnNr: column in which to insert the coin
 *
 * Returns:
 * 	whether the play action is completed
 * 	TODO: remove this, has no added value
 * 			instead do error handling
 */
int standardPlayLoop(uint8_t columnNr, int prevValue) // move to put a coin in a column
{
    int robotMove = 0;

    StackChoice = whatStackToEmpty();
    moveToPos(mx_storePos[StackChoice], mz_storePos[mem_StorageStack[StackChoice] - 1], MAX_DELTA_X); // storagestack-1 to convert to index
    waitFunction(100);

    GPIO_SetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);
    waitFunction(100);

    moveToPos(mx_columnPos[columnNr], MZ_TOP, MAX_DELTA_X);
    waitFunction(100);

    setHorizontalVerticalServo(SERVO_HORZ);
    waitFunction(100);

    moveToPos(mx_columnPos[columnNr], MZ_DROP, MAX_DELTA_X);
    waitFunction(100);

    GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN); //disable vacuum pump
    waitFunction(100);

    setHorizontalVerticalServo(GRIPPER_RELEASE_ANGLE);
    waitFunction(20);
    setHorizontalVerticalServo(SERVO_HORZ);
    waitFunction(100);

    robotMove = 0;

    // Check for cheating
    if (checkcoin() == 9)
    {
    	moveToPos(mx_columnPos[columnNr], MZ_TOP, MAX_DELTA_X); //move to column rotate height
    	setHorizontalVerticalServo(SERVO_VERT);
    	waitFunction(20);
    	performEndOfGameLogic();
		detect_error = 0;
		state = CMD_GAME_END;
		return prevValue;
    }
    else if (checkcoin() != -1)
	{
		robotMove = 1;
	}

    if (robotMove == 0)
    {
        GPIO_SetBits(DLED_PORT, DLEDS);
        input_error++;
        moveToPos(mx_columnPos[columnNr], MZ_TOP, MAX_DELTA_X); //move to column rotate height
        waitFunction(20);
        GPIO_ResetBits(DLED_PORT, DLED1);
        waitFunction(20);
        setHorizontalVerticalServo(SERVO_VERT);

        GPIO_ResetBits(DLED_PORT, DLED2);
        waitFunction(20);
        if (prevValue == 0)
        {
            prevValue = mz_storePos[mem_StorageStack[StackChoice] - 1];
            mz_storePos[mem_StorageStack[StackChoice] - 1] = mz_storePos[mem_StorageStack[StackChoice] - 1] - 1;
            GPIO_ResetBits(DLED_PORT, DLED3);
        }
        else
        {
            mz_storePos[mem_StorageStack[StackChoice] - 1] = prevValue;
            prevValue = -1;
            mem_StorageStack[StackChoice]--;
            GPIO_ResetBits(DLED_PORT, DLED4);
        }

        return prevValue;
    }
    else
    {
        for (int k = 0; k < 7; k++)
        {
            mem_Board_old[k] = mem_Board[k];
        } //update board
        mem_StorageStack[StackChoice]--;
    }

    moveToPos(mx_columnPos[columnNr], MZ_TOP, MAX_DELTA_X); //move to column rotate height
    waitFunction(50);

    setHorizontalVerticalServo(SERVO_VERT);
    waitFunction(50);

    if (prevValue != 0)
    {
        mz_storePos[mem_StorageStack[StackChoice] - 1] = prevValue;
        prevValue = 0;
    }
    StackChoice = whatStackToEmpty();
    moveToPos(mx_storePos[StackChoice], MZ_STORE + 20, MAX_DELTA_X);

    return 0;
}

/**
 * performEndOfGameLogic: clear board and sort chips
 *
 * The end of game logic consists of a few steps:
 * 1: Empty one column
 * 2: Pick up chip from dump stack
 * 3: Use RGB sensor to determine color of chip
 * 4: Based on color: shoot to human or place in storage stack
 * 5: Go back to 2 until dump stack is empty
 * 6: Go back to 1 until last column is empty
 */
void performEndOfGameLogic()
{
    EnableMotors(1, 1, 1);
    int rgb = 0;
    int currentPos = 0;
    int temp = 0;
    uint8_t nColumnsEmpty = 0;
    uint8_t nChipsDump = 0;
    sens = 1;

    while (nColumnsEmpty < 7)
    {
        currentPos = mx_FullPos / Mem_PositionConstant; //determine current pos in mm

        // Move to servo safe rotation position and rotate
        setEmptyingServoPosition(nColumnsEmpty);
        moveToPos(currentPos, MZ_DUMP_STACK + 60, MAX_DELTA_X);

        nChipsDump = mem_Board[empty_mem[nColumnsEmpty]];
        mem_Board[empty_mem[nColumnsEmpty]] = 0;

        while (nChipsDump > 0)
        {
            moveToPos(MX_DUMP_STACK, MZ_DUMP_STACK, MAX_DELTA_Z);
            waitFunction(25);
            setHorizontalVerticalServo(SERVO_VERT);

            waitFunction(25);
            moveToPos(MX_DUMP_STACK, mz_dumpPos[nChipsDump], MAX_DELTA_Z);
            waitFunction(25);
            GPIO_SetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);

            waitFunction(25);

            moveToPos(MX_FLIPPER, MZ_STORE + 20, MAX_DELTA_X); //RGB location  //MZ store is not always high enough depending on the accurancy
            waitFunction(25);
            moveToPos(MX_FLIPPER, MZ_FLIPPER, MAX_DELTA_X); //RGB location
            waitFunction(25);

            rgb = readRGBSensor();
            if (rgb == ROBOT_CHIP)
            {

                moveToPos(MX_FLIPPER, MZ_STORE + 30, MAX_DELTA_X); // Carry height
                waitFunction(25);

                moveToPos(mx_storePos[whatStackToFill()], MZ_STORE + 10, MAX_DELTA_X); // To correct storage stack {0,1,2}
                waitFunction(50);

                GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);
                waitFunction(100);								// Vacuum release

                mem_StorageStack[whatStackToFill()]++;
            }
            else if (rgb == HUMAM_CHIP)
            {
                GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN);
                waitFunction(100);								// Vacuum release

                moveToPos(MX_FLIPPER, MZ_STORE + 5, MAX_DELTA_X); // Carry height
                waitFunction(50);

                controlSolenoid();
            }
            else
            {                                                //no coin picked up retry
                GPIO_ResetBits(VAC_PUMP_PORT, VAC_PUMP_PIN); //vacuum off
                if (temp != 0)
                {
                    temp = mz_dumpPos[nChipsDump];
                }
                mz_dumpPos[nChipsDump] = (mz_dumpPos[nChipsDump] - 1);
                continue;
            }

            if (temp != 0)
            {
                mz_dumpPos[nChipsDump] = temp;
                temp = 0;
            }
            nChipsDump -= 1;
        }

        nColumnsEmpty += 1;
    }
    CloseEmptyingServo();
    for (int k = 0; k < 7; k++)
    {
        mem_Board_old[k] = 0;
    }
    currentPos = mx_FullPos / Mem_PositionConstant;
    moveToPos(currentPos, MZ_DUMP_STACK + 60, MAX_DELTA_X);
    moveToPos(mx_storePos[whatStackToFill()], MZ_DUMP_STACK + 50, MAX_DELTA_X);
    moveToPos(mx_storePos[whatStackToFill()], MZ_STORE + 10, MAX_DELTA_X);
}
