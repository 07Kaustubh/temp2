#include "main.h"
#include "stdarg.h"
#include "gyroscope.h"
#include "Accelometer.h"

uint16_t Xaxis=0,Yaxis=0;
extern SPI_HandleTypeDef hspi1;
uint32_t array[4];int i=0;
int once =1;

#define THRESHOLD 6
#define negative_THRESHOLD -7
#define SENSITIVITY_8G      0.244f
#define NUM_SAMPLES 	50
#define IDLE_THRESHOLD 1.00f // Define a threshold to determine if the sensor is idle
#define IDLE_DURATION 10000 // Duration in milliseconds for which the sensor should be idle

int16_t accelX, accelY, accelZ,accelMagnitude_X_Y,accelMagnitude_ALL;
int16_t accel_valueX,accel_valueY,accel_valueZ;
float accelX_sum = 0, accelY_sum = 0, accelZ_sum = 0;
float accelX_avg = 0, accelY_avg = 0, accelZ_avg = 0;
float accelX_offset = 0, accelY_offset = 0, accelZ_offset = 0;
float accelX_values[NUM_SAMPLES] = {0};
float accelY_values[NUM_SAMPLES] = {0};
float accelZ_values[NUM_SAMPLES] = {0};
int sampleIndex = 0;
int sampleCount = 0;
unsigned long lastUpdateTime = 0;
bool isIdle = false;
int HAT =5, HBT=5;

void accel_raw() {
	accelX = readAccelerometerAxis(LSM6DSL_OUTX_L_XL, LSM6DSL_OUTX_H_XL);
	accelY = readAccelerometerAxis(LSM6DSL_OUTY_L_XL, LSM6DSL_OUTY_H_XL);
	accelZ = readAccelerometerAxis(LSM6DSL_OUTZ_L_XL, LSM6DSL_OUTZ_H_XL);

	accel_valueX = (float)accelX * SENSITIVITY_8G * (9.80665f / 1000.0f);
	accel_valueY = (float)accelY * SENSITIVITY_8G * (9.80665f / 1000.0f);
	accel_valueZ = (float)accelZ * SENSITIVITY_8G * (9.80665f / 1000.0f);
//printc(1000,"accelValueX:%d\naccelValueY:%d\naccelValueZ:%d\n"
//		,accel_valueX,accel_valueY,accel_valueZ);	// Update the running sums and averages

	//	accelX_sum -= accelX_values[sampleIndex];
//	accelY_sum -= accelY_values[sampleIndex];
//	accelZ_sum -= accelZ_values[sampleIndex];
//
//	accelX_values[sampleIndex] = accel_valueX;
//	accelY_values[sampleIndex] = accel_valueY;
//	accelZ_values[sampleIndex] = accel_valueZ;
//
//	accelX_sum += accel_valueX;
//	accelY_sum += accel_valueY;
//	accelZ_sum += accel_valueZ;
//
//	sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
//	if (sampleCount < NUM_SAMPLES) {
//		sampleCount++;
//		accelX_avg = accelX_sum / sampleCount;
//		accelY_avg = accelY_sum / sampleCount;
//		accelZ_avg = accelZ_sum / sampleCount;
//
//		// Compute the offset based on the average of the last  samples
//		accelX_offset =  accelX_avg;
//		accelY_offset =  accelY_avg;
//		accelZ_offset =  accelZ_avg;
//
//	}
	// Apply the offset to the current values
	//    accel_valueX = accel_valueX -accelX_offset;
	//    accel_valueY = accel_valueY -accelY_offset;
	//    accel_valueZ =accel_valueZ -accelZ_offset;
	//    IMU_Events(accel_valueX,accel_valueY,accel_valueZ);
	//	if(once==1)
	//	{00
	//		once=0;
	//		 flag = Movement_Calibration();
	//	}
}


void IMU_Events(int16_t AcX, int16_t AcY, int16_t AcZ)
{
	if(AcX > HAT)
	{
		generateData("HA");
	}
	if(AcY > HBT)
	{
		generateData("HB");
	}

}


void LSM6DSL_EnableAccelerometer(){
	uint8_t CONTROL_ACCEL  = 0x10;
	uint8_t ctrlaccel_c_value = 0xAC;

	LL_GPIO_ResetOutputPin(GPIOA, SPI1_CS_Pin);

	HAL_SPI_Transmit(&hspi1,&CONTROL_ACCEL, 1, HAL_MAX_DELAY); // Send register address
	HAL_SPI_Transmit(&hspi1, &ctrlaccel_c_value, 1, HAL_MAX_DELAY);   // Send value

	LL_GPIO_SetOutputPin(GPIOA, SPI1_CS_Pin);
}

int16_t readAccelerometerAxis(uint8_t regLow, uint8_t regHigh){
	uint8_t txData =  regLow | 0x80 ;
	uint16_t rxData;

	LL_GPIO_ResetOutputPin(GPIOA, SPI1_CS_Pin);

	HAL_SPI_Transmit(&hspi1, &txData, 1, 100);
	HAL_SPI_Receive(&hspi1, &rxData, 2, 100);

	LL_GPIO_SetOutputPin(GPIOA, SPI1_CS_Pin);

	return rxData;
}
int Movement_Calibration(void)
{
	uint16_t px=0,py=0,nx=0,ny=0;
	while(HAL_GetTick()<=10000){//this is function when imu is sattle like y direction.
		accel_raw();
		if (fabs(accel_valueX) > THRESHOLD || fabs(accel_valueY) > THRESHOLD||fabs(accel_valueX) < negative_THRESHOLD || fabs(accel_valueY) < negative_THRESHOLD){
			if (fabs(accel_valueX) > fabs(accel_valueY)) {

				if (accel_valueX > 0 && accel_valueX>THRESHOLD) {
					px++;
					printc(10,"Moving along the +VE X-axis\n");
				}
				else if(accel_valueX < negative_THRESHOLD){
					nx++;
					printc(10,"Moving along the -VE X-axis\n");
				}
				Xaxis++;
			}
			else {
				if (accel_valueY > 0 && accel_valueY>THRESHOLD) {
					py++;
					printc(10,"Moving along the +VE Y-axis\n");
				}
				else if(accel_valueY < negative_THRESHOLD){
					ny++;
					printc(10,"Moving along the -VE Y-axis\n");
				}
				Yaxis++;
			}
		}
	}
	if(Xaxis > Yaxis)
	{
		printc(10,"Moving along the X-axis\n");
		if(px>nx)
		{
			printc(10,"Moving along the +VE X-axis\n");
		}
		else
		{
			printc(10,"Moving along the -VE X-axis\n");
		}
		return 1;
	}
	else
	{
		printc(10,"Moving along the Y-axis\n");
		if(py>ny)
		{
			printc(10,"Moving along the +VE Y-axis\n");
		}
		else
		{
			printc(10,"Moving along the -VE Y-axis\n");
		}
		return 0;
	}
	return 2;
}
