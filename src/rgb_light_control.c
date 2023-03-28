/*
 * rgb_light_control.c
 *
 *  Created on: 4 Sep 2017
 *      Author: chanselaar
 *      goal: 	initiate sensor and read color
 */

#include "rgb_light_control.h"
#include "i2c.h"
#include "motorControlInterrupt.h"
#include <math.h>
#include "vars.h"

/**
 * initRGBSensor: initialize sensor
 *
 * First enable sensor
 * Next, set timing register
 */
void initRGBSensor(void)
{

	//	buffer[0] = RGB_COMMAND_REG | RGB_REG_ENABLE;	// When sending command, always set command_reg bit
	//	buffer[1] = 0x03;								// 0x03 : Power on and enable ADC
	//	sendData(I2C_ADD_RGB, 2);
	//	waitFunction(2);								// Delay at least 2.4 ms

	// start up the sensor.
	enableRGBSensor();

	// Set timing register (ADC integration time)
	// Integration time can be set in steps of 2.4 ms.
	// 0xFF = 2.4 ms
	// 0x00 = 700 ms
	// 0xEE = 238; (256 - 238) * 2.4 = 43.2 ms
	buffer[0] = RGB_COMMAND_REG | RGB_REG_TIMING;

	buffer[1] = 0xEE;
	sendData(I2C_ADD_RGB, 2);

	waitFunction(8); // wait for the LED to turn on
}

/**
 * enableRGBSensor: enable the sensor
 *
 */
void enableRGBSensor(void)
{
	buffer[0] = RGB_COMMAND_REG | RGB_REG_ENABLE; // When sending command, always set command_reg bit
	buffer[1] = RGB_ENABLE_PON | RGB_ENABLE_AEN;  // Power on and enable ADC
	sendData(I2C_ADD_RGB, 2);
	waitFunction(2);
}

/**
 * disableRGBSensor: disable the sensor
 *
 * This command puts the sensor in low power mode
 *
 */
void disableRGBSensor(void)
{
	uint8_t reg = 0x00;
	buffer[0] = RGB_COMMAND_REG | RGB_REG_ENABLE; // When sending command, always set command_reg bit
	buffer[1] = reg & ~(RGB_ENABLE_PON | RGB_ENABLE_AEN);
	sendData(I2C_ADD_RGB, 2);
	waitFunction(2);

	setLEDRGBSensor(0);
}

void setLEDRGBSensor(int value)
{

	buffer[0] = RGB_COMMAND_REG | RGB_REG_ENABLE; // When sending command, always set command_reg bit
	if (value)
		buffer[1] = RGB_ENABLE_AIEN; // 0x03 : Power on and enable ADC
	else
		buffer[1] = ~RGB_ENABLE_AIEN;

	sendData(I2C_ADD_RGB, 2);
	waitFunction(8); // Delay at least 2.4 ms
}

/**
 * readRGBSensor: check if yellow or red chip is present
 *
 * Checks whether RGB values are within range for either yellow or red chip
 * If neither is found, repeat 24 times, otherwise send to human
 *
 * Returns:
 * 	1: robot chip
 * 	0: human chip
 */
int readRGBSensor(void)
{
	struct Color sens_RGBOut;
	int robotCoin = -2;
	int t_it_RGB = 0;
	uint16_t hue = 0;

	// initiate sensor
	initRGBSensor();

	while (robotCoin == -2) // loop until we have reached the max
	{
		sens_RGBOut = queryRGBSensor();

		// get hue value (dominant wavelength) out of RGB sensor readings
		hue = getHue(sens_RGBOut.r, sens_RGBOut.g, sens_RGBOut.b);

		if (hue >= 35 && hue <= 75)
		{
			// it is yellow
			robotCoin = 0; //for endurance test else 0
			geel++;
		}
		if (hue >= 340 && hue <= 360)
		{
			// it is red
			robotCoin = 1;
			rood++;
		}

		t_it_RGB++;
		waitFunction(2);
		if (t_it_RGB > 24)
		{
			//error = -1;
			robotCoin = 3; //for endurance test else -1; // dump to human, let the humanoid sort it out (altered to recheck)
			rgb_error++;
		}
	}
	// if 1: it is one for the robot
	// if 0: one for the humanoid

	//disableRGBSensor();

	return robotCoin;

	//	return error ? -1 : robotCoin;
}

/**
 * queryRGBSensor: retrieve information from sensor
 *
 * Retrieves 16-bit number for red, green and blue
 *
 * Returns:
 * 	Color struct containing r, g and b
 */
struct Color queryRGBSensor(void)
{

	/* Read RGB values */
	uint8_t low, high; // temporary
	struct Color color;

	// Read red value
	buffer[0] = RGB_COMMAND_REG | RGB_RED_LOW;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	low = buffer[0];
	buffer[0] = RGB_COMMAND_REG | RGB_RED_HIGH;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	high = buffer[0];
	color.r = (high << 8) | low;

	// Read green value
	buffer[0] = RGB_COMMAND_REG | RGB_GREEN_LOW;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	low = buffer[0];
	buffer[0] = RGB_COMMAND_REG | RGB_GREEN_HIGH;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	high = buffer[0];
	color.g = (high << 8) | low;

	// Read blue value
	buffer[0] = RGB_COMMAND_REG | RGB_BLUE_LOW;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	low = buffer[0];
	buffer[0] = RGB_COMMAND_REG | RGB_BLUE_HIGH;
	sendReceiveData(I2C_ADD_RGB, 1, 1);
	high = buffer[0];
	color.b = (high << 8) | low;

	return color;
}

/**
 * getHue: retrieve information from sensor
 *
 * Converts an RGB value to Hue spectrum (dominant wavelength)
 *
 * Inputs:
 * red:		red component of RGB
 * green:	green component of RGB
 * blue:	blue component of RGB
 *
 * Returns:
 * 	hue value in the range of 0 to 360
 */
uint16_t getHue(uint16_t red, uint16_t green, uint16_t blue)
{
	uint16_t max = max(max(red, green), blue);
	uint16_t min = min(min(red, green), blue);
	float hue = 0;

	// grey:
	if (min == max)
	{
		return 0;
	}

	if (max == red)
	{
		hue = (green - blue) / (float)(max - min);
	}
	else if (max == green)
	{
		hue = 2.0f + (blue - red) / (float)(max - min);
	}
	else
	{
		hue = 4.0f + (red - green) / (float)(max - min);
	}

	hue *= 60;
	// if hue is negative, add 360
	if (hue < 0)
		hue += 360;

	// round hue value and return
	return roundf(hue);
}
