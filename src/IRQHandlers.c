#include "IRQHandlers.h"
#include "vars.h"
#include "checkForCoinPlay.h"
#include "motorPIDs.h"
#include "motorpwm.h"
#include "servoControl.h"
#include "uart.h"

int x = 0;
double arr[1000];


/* INTERRUPT HANDLER FUNCTIONS */
/**
 * TIM7_IRQHandler: motor control interrupt handler
 * Runs at 100 Hz
 *
 * Responsible for:
 * 		Checking coin detector state
 * 		Executing control loop
 * 		Updating the counter for the wait function
 * Once every 10 iterations:
 * 		Control vac. pump
 * 		(init rgb sensor)
 * 		(read rgb sensor)
 * 		Reading proximity sensor
 * 		Controlling the flipper
 */

void TIM7_IRQHandler()
{
	/* Check coin detector */
	if (sens == 1)
	{
		Sens_CoinEntry = coinInsertDetection();


		if(Sens_CoinEntry > 0 && Sens_CoinEntry < 8)
		{
			mem_Board[Sens_CoinEntry - 1]++;
		}
	}

	if(moveServo != 0)
	{
		SetHVServoTimer();
	}

	/* Execute control loop */
	motorPIDPositionControl();

	TIM7IQ_ItCounter++;		  // iterate the counter
	if (TIM7IQ_ItCounter > 9) // max loop counter
		TIM7IQ_ItCounter = 0;

	/* Update the timer counter for custom wait times */
	TIM7_CustomTimerCounter++;

	/* Finally, clear the interrupt flag */
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
}

/**
 * UART4_IRQHandler: reception over serial communication port
 * Runs whenever a byte is received (highest priority)
 *
 * Reads received bytes to a buffer.
 * If the main loop is already processing a command, a new one cannot be received.
 * In that case, check for ack/nack reception.
 */

void UART4_IRQHandler() // Serial communication
{
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		/* Only process new command when done with previous one */
		if (!processingCommand)
		{
			USART_ClearITPendingBit(UART4, USART_IT_RXNE);
			uartReadByte();
		}
		else
		{
			/* Check if ack/nack is received */
			if (uartReadAckNack())
				USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		}
	}
}

/* Handle PB0 interrupt - ENDSWITCH X-direction */
void EXTI0_IRQHandler() // END switch triggered
{
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{

		/* Disable motors when this interrupt is triggered TODO: Handle differently (reset?), motor disable for now (safety) */
		DisableMotors(1, 1, 1);

		/* Clear interrupt flag */

	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/* Handle PB1 interrupt - ENDSWITCH Z-direction */
void EXTI1_IRQHandler() // END switch triggered
{
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{

		/* Disable motors when this interrupt is triggered TODO: Handle differently (reset?), motor disable for now (safety) */
		DisableMotors(1, 1, 1);

		/* Clear interrupt flag */

	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/*Encoder under/overflow handler*/
void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		if (TIM3->CR1 & 0x10)
		{
			overflowX -= 1;
		}
		else
		{
			overflowX += 1;
		}

	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
/*Encoder under/overflow handler*/
void TIM4_IRQHandler()
{

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{

		if (TIM4->CR1 & 0x10)
		{
			overflowZ -= 1;
		}
		else
		{
			overflowZ += 1;
		}

	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}
