/*
 * logic.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Rodchananat
 */

#ifndef INC_LOGIC_H_
#define INC_LOGIC_H_

#include "mydefines.h"

int _fallTick = 0;
int _hasFallen = 0;

// main check falling logic
int checkFalling(float ax, float ay, float az, float gx, float gy, float gz) {
	float aMag = ax*ax + ay*ay + az*az;
	float gMag = gx*gx + gy*gy + gz*gz;
	if (aMag > FALL_ACCEL_MAGNITUDE && aMag < 6000 || gMag > FALL_GYRO_MAGNITUDE && gMag < 40000){
		_fallTick++;
		if (_fallTick >= 3) _hasFallen = 1;
		return 1;
	}
	_fallTick = 0;
	return 0;
}


int hasFallen() {
	return _hasFallen;
}

int resetFallen() {
	_hasFallen = 0;
}



int readMessage(UART_HandleTypeDef* handle, char *buffer, int maxLength) {
	int length = 0;
	while (HAL_UART_Receive(handle, &buffer[length], 1, 10) == HAL_OK && length < maxLength) length++;
	return length;
}



#endif /* INC_LOGIC_H_ */
