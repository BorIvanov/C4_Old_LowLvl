/*
 * CheckForCoinPlay.c
 *
 *  Created on: 1 Sep 2017
 *      Author: chanselaar
 *      Goal:	Query the light sensors via I2C, to find out if you had a passing coin as played by a human
 *      		Note that you should recognise when a coin that you have played has entered the system
 *
 */

#include "checkForCoinPlay.h"
#include "vars.h"
#include "i2c.h"
#include "stm32f30x.h"


/**
 * coinInsertDetection: check with light gates if coin is inserted
 *
 * returns:
 * 	0: no move
 * 	1-7: inserted into column n
 * 	9: error
 */
int coinInsertDetection(void)
{
	int output = 0;

	/* Query the processor with the I2C connection for the light gates */
	int gateNew = queryLightGate();

	if (gateNew == 9)
	{
		detect_error++;
		return 9;
	}

	/* Query the logic: if the same as the prior gate, not debounced yet.  */
	if (gateNew == -1 && gateOld <= 7 && gateNew != gateOld)
	{
		output = gateOld;
	}

	gateOld = gateNew;

	return output;
}

/**
 * whatStackToFill: determine which storage stack to fill
 *
 * Always start filling the first stack, then the second, finally the last.
 * Each stack can hold max. 7 chips
 *
 * Returns:
 * 	-1: All stacks are full
 * 	0-2: Number of stack
 */

int whatStackToFill(void)
{
	int stack = 2;
	for (int i = 0; i < 3; i++) // loop through all options
	{
		if (mem_StorageStack[i] < 7) // assert if not filled
		{
			stack = i; // if not filled, use this stack
			break;
		}
	}
	return stack;
}

/**
 * whatStackToEmpty: determine which storage stack to empty
 *
 * Always start emptying the last stack, then the second, finally the first.
 *
 * Returns:
 * 	-1: All stacks are empty
 * 	0-2: Number of stack
 */
int whatStackToEmpty(void)
{
	int stack = 2;
	for (int i = 2; i >= 0; i--) // loop through all options
	{
		if (mem_StorageStack[i] > 0) // assert if not empty
		{
			stack = i; // if not empty, use this stack
			break;
		}
	}
	return stack;
}

/**
 * queryLightGate: read coin detector value.
 *
 * Reads coin detector using I2C. Returns column where something is present.
 *
 * Returns:
 * -1: All gates are free
 * 1-7: Stack where gate is blocked
 * 9: More than one gate is blocked
 */
int queryLightGate(void)
{
	uint8_t cd;
	int found = 0;
	int returnValue = 0;
	/* Retrieve input data */
	buffer[0] = 0;
	sendReceiveData(I2C_ADD_CD, 1, 1);

	/* Process data */
	cd = buffer[0];

	/* Remove LSB: is always 1 */
	cd &= ~0x01;

	/* Check if empty: */
	if (!cd)
		return -1;

	if (cd == 0x02)
	{
		return 7;
	}
	if (cd == 0x04)
	{
		return 6;
	}
	if (cd == 0x08)
	{
		return 5;
	}
	if (cd == 0x10)
	{
		return 4;
	}
	if (cd == 0x20)
	{
		return 3;
	}
	if (cd == 0x40)
	{
		return 2;
	}
	if (cd == 0x80)
	{
		return 1;
	}

	//More than one coin
	if ((cd >> 1) & 1)
	{
		mem_Board[7 - 1]++;
	}
	if ((cd >> 2) & 1)
	{
		mem_Board[6 - 1]++;
	}
	if ((cd >> 3) & 1)
	{
		mem_Board[5 - 1]++;
	}
	if ((cd >> 4) & 1)
	{
		mem_Board[3 - 1]++;
	}
	if ((cd >> 5) & 1)
	{
		mem_Board[4 - 1]++;
	}
	if ((cd >> 6) & 1)
	{
		mem_Board[1 - 1]++;
	}
	if ((cd >> 7) & 1)
	{
		mem_Board[2 - 1]++;
	}
	sens = 0;
	return 9;
}

/*Function to check if a new coin has been tossed in*/
int checkcoin(void)
{
	int column = -1;
	int amountOfColumnsChanged = 0;
	for (int i = 0; i < 7; i++)
	{
		if (mem_Board[i] - mem_Board_old[i] > 0)
		{
			column = i;
			amountOfColumnsChanged++;
		}
	}
	if (amountOfColumnsChanged == 1)
	{
		column++;
	}
	else if (amountOfColumnsChanged > 1)
	{
		column = 9;
	}
	return column;
}
