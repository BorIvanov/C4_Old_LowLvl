/*
 * initSysClock.c
 *
 *  Created on: 30 Aug 2017
 *      Author: chanselaar
 *  	Goal: 	ensuring that the correct clock speed is used.
 */
#include "stm32f30x.h"

/**
 * initSysClock: Initialize the system clock to run at the correct frequency
 */
void initSysClock() // sets the system clock as PLL from oscillator
{
	/* In HSEConfig the system changes specific bytes of the CR register
	 * Specifically in this case, the bytes adjusted are
	 * CR register byte 2 (Bits[23:16]) base address
	 * These refer to the bytes of the clock control register (RCC_RC)
	 * described on pages 137 of
	 * http://www.st.com/content/ccc/resource/technical/document/reference_manual/4a/19/6e/18/9d/92/43/32/DM00043574.pdf/files/DM00043574.pdf/jcr:content/translations/en.DM00043574.pdf
	 *
	 * The logic therein described has been checked by Jeroen Wilbers and Caspar Hanselaar on 28/08/2017
	 *
	 * */
	// reset the timer to initialization parameters. Essentially this shouldn't change a thing, but
	// it may be useful to clear some remaining errors in memory
	RCC_DeInit();
	// Now it is time to start to turn it on.
	RCC_HSEConfig(RCC_HSE_ON);						 // this function calls a function in stm32f30x_rcc.c
	ErrorStatus HSEStatus = RCC_WaitForHSEStartUp(); // next we assess the current status of the HSE timer

	if (HSEStatus == SUCCESS) // if HSE startup ok, continue with PLL
	{
		/* Setting the FLASH latency and prefetch buffer due to the change of clock speed.
		 * According to http://.st.com/content/ccc/resource/technical/document/reference_manual/4a/19/6e/18/9d/92/43/32/DM00043574.pdf/files/DM00043574.pdf/jcr:content/translations/en.DM00043574.pdf
		 * the prefetch buffer enable helps the CPU to attain max performance if it is clocked higher.
		 * At those points, we also need to increase the memory latency to higher settings
		 * for 0 to 24 MHz, we need to set it to 0 (and set the prefetch to off)
		 * for >24 to 48 MHz, we need to set it to 1 (bit 001)
		 * And for >48 to 72 MHz, we need to set it to 2 (bit 010)
		 *
		 * In contrast to the prior functions, the address offset is 0 here, meaning that
		 * the first byte is rewritten.
		 * */

		/* Enable Prefetch Buffer and set Flash Latency */
		FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
		/* a message consisting of 00010010 is written.
		 * This translates to the fifth bit turning on the prefetch,
		 * and the second bit turning latency to max
		 * */

		/*
		 * Next up, we will start to set up the different multipliers for the system clocks.
		 * First up, is the AHB prescalar. This clock divider sits between the SYSCLK that sets the i2c
		 * and i2ssrc up, and the AHB busses.
		 * In our case, we would like this to not scale any further, as we want the system to be set up
		 * at the maximum frequency.
		 * The next one is the APB2 prescalar. these busses can run up to 72 MHz, the same max rate as
		 * the system max rate. As such, we would also like to have them run at this rate, to reduce the
		 * chance that this becomes a limiting factor
		 * Finally, the APB1 prescalar is set. This bus can hanlde up to 36 MHz, or half the system clock
		 * frequency. Therefore, we divide this timer by 2, to make sure there are no conflicts here.
		 */
		/* HCLK = SYSCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1; // SYSCLK not divided
		/* PCLK2 = HCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1; // APB1 divide is set to 1
		/* PCLK1 = HCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2; // APB2 divide is set to 2

		/*  PLL configuration */
		/*
		 * Step 1: reset the PLL system, by setting the PLL source, PLL XTPRE (external Pre-division for HSE clock)
		 * and the post PLL multiplier to zeros, by setting it to "and zero" (&=)
		 */
		RCC->CFGR &= (uint32_t)((uint32_t) ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));

		/*
		 * Next, we set the source and multiplier as we see fit, by setting the or bit action and the appropriate
		 * source and multiplier.
		 *  first bit set: RCC_CFGR_PLLSRC_PREDIV1
		 *  	effect: selecting PreDiv as clock source. As the HSE must pass through the PreDiv, this is the correct one
		 *  second bit set: RCC_CFGR_PLLXTPRE_PREDIV1_Div2
		 *  	effect: ensuring that the HSE clock is divided by 2 before being fed into the PLL.
		 *  third bit set: RCC_CFGR_PLLMULL9
		 *  	effect: multiplying the clock input by 9.
		 *  Net expected result: a system clock of 72 MHz.
		 */
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL9); //Caspar

		/* Enable PLL */

		/*
		 * Finally, we want to make sure that we turn on the PLL as the main source for the system clock.
		 * To do so, we set the following bit in the main clock register CR.
		 * This will cause the 25th bit to go to ON (note zero-base indexing makes it bit no 24).
		 */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till PLL is ready */
		/*
		 * According to the reference manual, it may take up to 6 clock cycles to get a PLL ready sign from the clock.
		 * We therefore wait here until we are sure the PLL started correctly
		 */
		while ((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Select PLL as system clock source */
		/*
		 * Next, we must set the PLL as the system clock source. We first will write a "and equal to" bit action
		 * of not active on both the bit for HSE source, and PLL source. This effectively sets it to HSI source.
		 * Next, we will set the PLL active bit (no 2, or zero-based indexing no 1) to be "on".
		 * This will have finally set the system clock to use the PLL as input.
		 */
		RCC->CFGR &= (uint32_t)((uint32_t) ~(RCC_CFGR_SW)); // set both HSE and PLL "on" values to zero, activating HSI
		RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;				// set the PLL value to "on"

		/* Wait till PLL is used as system clock source */
		/*
		 * now we have reached the final step: we must now wait until the SWS (the read-only switch result)
		 * has indicated that we have switched over to the PLL as the system clock. The values of these next to bits
		 * (bits 3 and 4, or zero-based-indexing bits 2 and 3) should be the same.
		 * So if we start a while loop, that will continue while the PLL switch bit is unequal to the Switch set bit,
		 * we should not be allowed to continue until it has properly been set!
		 */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
		{
		}
		/*
		 * CLOCK SETUP COMPLETE!
		 * */
	}
	else
	{
		while (1)
		{
			; // inf loop if in error. This prevents it from continuing in boot if the HSE timre doesn't start correctly!
		}
	}
}
