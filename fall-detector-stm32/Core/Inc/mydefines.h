/*
 * defines.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Rodchananat
 */

#ifndef INC_MYDEFINES_H_
#define INC_MYDEFINES_H_

// threshold for fall
#define FALL_ACCEL_MAGNITUDE 140
#define FALL_GYRO_MAGNITUDE 20000

#define RED_BOARD_HANDLE &huart2
#define NODEMCU_HANDLE &huart3

#define GYRO_HANDLE &hi2c1

#define GYRO_STATUS_LED_PIN GPIOD, LD5_Pin
#define FALL_STATUS_LED_PIN GPIOD, LD3_Pin
#define BUZZER_OUT_PIN GPIOD, GPIO_PIN_11




#define MPU6050_I2C_ADDR 0xD0

#endif /* INC_MYDEFINES_H_ */