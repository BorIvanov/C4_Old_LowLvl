/*
 * init.h
 *
 *  Created on: 12 Apr 2019
 *      Author: Jasper de Weger
 */

#ifndef IRQHANDLERS_H_
#define IRQHANDLERS_H_

//int x = 0;
//double arr[1000];

/* Local interrupt function prototypes */
void TIM7_IRQHandler();  // Timer7 interrupt handler (100Hz)
void UART4_IRQHandler(); // UART interrupt handler function
void EXTI0_IRQHandler(); // EXTI0 input interrupt handler (endswitch X)
void EXTI1_IRQHandler(); // EXTI1 input interrupt handler (endswitch Z)
void TIM3_IRQHandler();  //encoder interrupt handler
void TIM4_IRQHandler();  //encoder interrupt handler

#endif /* IRQHANDLERS */
