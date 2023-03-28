/*
 * proximityTest.c
 *
 *  Created on: 4 Sep 2017
 *      Author: chanselaar
 */

// send request to test proximity
int reqProximTest(void)
{

	return 0; // 0 if ok, -1 if no ack received before time-out
}

int testProxim(void)
{
	int aWake = reqProximTest(); // test the sensor, see if awake

	//TODO perform logic to test here

	return 0; // 1 if something is there, 0 if not
}
