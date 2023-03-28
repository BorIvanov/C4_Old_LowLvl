/*
 * solenoidControl.h
 *
 *  Created on: 23 April 2017
 *      Author: jajansen
 */

#ifndef SOLENOIDCONTROL_H_
#define SOLENOIDCONTROL_H_

#include "vars.h"

void initSolenoid();
void controlSolenoid();
void setSolenoidDutyCycle(uint32_t dutycycle);

#endif /* SOLENOIDCONTROL_H_ */
