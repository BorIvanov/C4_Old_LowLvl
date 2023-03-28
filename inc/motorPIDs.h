/*
 * MainMotorPIDs.h
 *
 *  Created on: 31 Aug 2017
 *      Author: chanselaar
 */

#ifndef MOTORPIDS_H_
#define MOTORPIDS_H_

#include "vars.h"

int32_t unwrapEncDelta(int32_t delta);
double PID(double Kp, double Kd, double Ki, double period, int32_t err, int32_t delta, double *intCount);
void motorPIDPositionControl();
void motorPIDSpeedControl();
void moveToXPos(int16_t target, double maxDelta);
void moveToZPos(int16_t target, double maxDelta);
void moveToPos(int16_t targetX, int16_t targetZ, double maxDelta);
void calcAccelration();
int calcDirection(int32_t currentPos, int32_t targetPos);
int32_t checkPosDelta(int32_t targetPos, int32_t attainedPos);
double clampValue(double value, double minvalue, double maxvalue);
int32_t calcHalfPosTarget(int32_t currentPos, int32_t targetPos);

#endif /* MOTORPIDS_H_ */
