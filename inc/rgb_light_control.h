/*
 * rgb_light_control.h
 *
 *  Created on: 4 Sep 2017
 *      Author: chanselaar
 */

#ifndef RGB_LIGHT_CONTROL_H_
#define RGB_LIGHT_CONTROL_H_

#include "stm32f30x.h" // base board parameters and settings --> adjusted HSE_VALUE

/* Define macro that finds maximum of two numbers */
#define max(a, b) \
	({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

/* Define macro that finds minimum of two numbers */
#define min(a, b) \
	({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/* Define thresholds for colour recognition */
#define ROBOT_RED_LOWER 100
#define ROBOT_YELLOW_UPPER 100
#define HUMAN_RED_LOWER 200
#define HUMAN_YELLOW_LOWER 100
#define BLUE_UPPER 150

/* Define registers of RGB sensor */
#define RGB_REG_ENABLE 0x00
#define RGB_REG_TIMING 0x01
#define RGB_REG_CONTROL 0x0F
#define RGB_RED_LOW 0x16 /* Red channel data */
#define RGB_RED_HIGH 0x17
#define RGB_GREEN_LOW 0x18 /* Green channel data */
#define RGB_GREEN_HIGH 0x19
#define RGB_BLUE_LOW 0x1A /* Blue channel data */
#define RGB_BLUE_HIGH 0x1B
#define RGB_ENABLE_AIEN 0x10
#define RGB_ENABLE_AEN 0x02 /* RGBC Enable - Writing 1 activates the ADC, 0 disables it */
#define RGB_ENABLE_PON 0x01 /* Power on - Writing 1 activates the internal oscillator, 0 disables it */

struct Color
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
};

// initialize system
void initRGBSensor(void);
void enableRGBSensor(void);
void setLEDRGBSensor(int value);

uint16_t getHue(uint16_t red, uint16_t green, uint16_t blue);

// read system
int readRGBSensor(void);

struct Color queryRGBSensor(void);

#endif /* RGB_LIGHT_CONTROL_H_ */
