/*
 * gyroscope.c
 *
 *  Created on: Apr 23, 2024
 *      Author: Shekhar Verma
 */
#include "main.h"
#include "Accelometer.h"
#include "gyroscope.h"
uint32_t gyro_array[4];int i_gyro =0;
extern SPI_HandleTypeDef hspi1;
float dt = 0.0001f,roll,pitch,yaw,gyroXvalue,gyroYvalue,gyroZvalue;
#define sensitivity_250  8.750f
int16_t gyroX,gyroY,gyroZ,gyromegnato;
uint8_t CONTROL_GYRO   =0x11;

void gyro_raw(void)
{
	gyroX = readGyroscopeAxis(LSM6DSL_OUTX_L_G, LSM6DSL_OUTX_H_G);
	gyroY = readGyroscopeAxis(LSM6DSL_OUTY_L_G, LSM6DSL_OUTY_H_G);
	gyroZ = readGyroscopeAxis(LSM6DSL_OUTZ_L_G, LSM6DSL_OUTZ_H_G);

	gyroXvalue = (float)gyroX * (sensitivity_250 / 1000.0f);
	gyroYvalue = (float)gyroY * (sensitivity_250 / 1000.0f);
	gyroZvalue = (float)gyroZ * (sensitivity_250 / 1000.0f);
	gyromegnato = sqrtf(powf(gyroXvalue, 2) + powf(gyroYvalue, 2));// + powf(gyroZvalue, 2));
//	calculateRollPitchYaw(gyroX, gyroY, gyroZ, dt, &roll, &pitch, &yaw);

//	gyro_array[i_gyro++] = gyroX;
//	gyro_array[i_gyro++] = gyroY;
//	gyro_array[i_gyro++] = gyroZ;
//	gyro_array[i_gyro++] = gyromegnato;
//	if(i_gyro==4){
//		i_gyro=0;
//		storeAccelMagnitudeToFlash(gyro_array);
//	  }
}
int16_t readGyroscopeAxis(uint8_t regLow, uint8_t regHigh)
{
	uint8_t txData =  regLow | 0x80;
	uint16_t rxData;

LL_GPIO_ResetOutputPin(GPIOA, SPI1_CS_Pin);

	HAL_SPI_Transmit(&hspi1, &txData, 1, 100);
	HAL_SPI_Receive(&hspi1, &rxData, 2, 100);

	LL_GPIO_SetOutputPin(GPIOA, SPI1_CS_Pin);

	return rxData;
}

void LSM6DSL_Enablegyro(void)
{
	int8_t ctrlgyro_c_value = 0x40;

	LL_GPIO_ResetOutputPin(GPIOA, SPI1_CS_Pin);

	HAL_SPI_Transmit(&hspi1,&CONTROL_GYRO , 1, HAL_MAX_DELAY); // Send register address
	HAL_SPI_Transmit(&hspi1, &ctrlgyro_c_value , 1, HAL_MAX_DELAY);   // Send value

	LL_GPIO_SetOutputPin(GPIOA, SPI1_CS_Pin);
}

void calculateRollPitchYaw(float gyroX, float gyroY, float gyroZ, float dt, float *roll_gyro, float *pitch_gyro, float *yaw_gyro)
{
	    *roll_gyro  += gyroX * dt;  // Roll (degrees)
	    *pitch_gyro += gyroY * dt;  // Pitch (degrees)
	    *yaw_gyro   += gyroZ * dt;  // Yaw (degrees)
}
