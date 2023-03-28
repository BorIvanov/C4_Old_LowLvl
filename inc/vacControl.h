/*
 * vacControl.h
 *
 *  Created on: 4 Sep 2017
 *      Author: chanselaar
 *
 *  Edited on 28-3-2018
 *  	Author: jajansen
 *
 */

#ifndef VACCONTROL_H_
#define VACCONTROL_H_

int vacControl(double pMeasure, double pDeltaMin, double pDeltaMax);
double vacPressMeasure();
void initPump();
void initValve();
void initVacSens();

#endif /* VACCONTROL_H_ */
