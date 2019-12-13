/*
 * logic.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Rodchananat
 */

#ifndef INC_LOGIC_H_
#define INC_LOGIC_H_

#include "mydefines.h"

int fallTick = 0;

int isFalling(float ax, float ay, float az, float gx, float gy, float gz) {
	float aMag = ax*ax + ay*ay + az*az;
	float gMag = gx*gx + gy*gy + gz*gz;
	if (aMag > FALL_ACCEL_MAGNITUDE || gMag > FALL_GYRO_MAGNITUDE){
		fallTick++;
		return 1;
	}
	fallTick = 0;
	return 0;
}


int fallTicks() {
	return fallTick;
}


void onFall() {
	setBuzzer(1);
	uartPrintf(RED_BOARD_HANDLE, "-");
}

void onHasFall() {
	setBuzzer(1);
	uartPrintf(RED_BOARD_HANDLE, "F");
}

void onNotFall() {
	setBuzzer(0);
	uartPrintf(RED_BOARD_HANDLE, ".");
}



#endif /* INC_LOGIC_H_ */
