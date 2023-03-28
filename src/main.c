/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

/* INLCUDES */
#include "vars.h"
#include "gamePlay.h"
#include "IRQHandlers.h"
#include "uart.h"
#include "init.h"
#include "motorpwm.h"
#include "motorControlInterrupt.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv) // main
{

	initProgram();

	/* Infinite loop where commands from PI are processed */
	while (state != STATE_SHUTDOWN)
	{

		/* Message received */
		if (uartStartSend && uartStopSend)
		{
			processingCommand = 1;
			command = uartPacket[1];
			payload = uartPacket[2];
			GPIO_ResetBits(DLED_PORT, DLED1);
			/* Respond to command */
			switch (command)
			{
			case CMD_INIT:
				EnableMotors(1, 1, 1);
				GPIO_ResetBits(DLED_PORT, DLED2);
				waitFunction(50);
				GPIO_ResetBits(DLED_PORT, DLED3);
				initGamePlay();
				GPIO_SetBits(DLED_PORT, DLEDS);
				break;
			case CMD_START: // Startup command

				startGamePlay();
				break;
			case CMD_DIFFICULTY: // Set difficulty
				//printf("\difficulty \n" );
				selectDifficulty();
				break;
			case CMD_COLUMN: // Wait for coin by human
				//GPIO_SetBits(DLED_PORT, DLED1 | DLED3);
				detectHumanInput();
				break;
			case CMD_INSERT_COIN: // Play command from totally not a robot rPI
				insertCoin();
				break;
			case CMD_GAME_END: // Somebody wins. Totally not a robot
				gameEnd();
				//GPIO_SetBits(DLED_PORT, DLEDS); // Turn all on to celebrate
				break;
			case CMD_CHEATER:
				gameEnd();
				break;
			}

			uartStartSend = 0;
			uartStopSend = 0;
			processingCommand = 0;
		}
	}
} // end main
