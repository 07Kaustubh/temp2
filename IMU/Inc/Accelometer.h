/*
 * Accelometer.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Shekhar Verma
 */

#ifndef INC_ACCELOMETER_H_
#define INC_ACCELOMETER_H_

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t accelMagnitude;
} AccelData;

//Accelerometer addresses
#define LSM6DSL_OUTX_L_XL       0x28   // Accelerometer X-axis Low Register (LSB)
#define LSM6DSL_OUTX_H_XL       0x29   // Accelerometer X-axis High Register (MSB)
#define LSM6DSL_OUTY_L_XL       0x2A   // Accelerometer Y-axis Low Register (LSB)
#define LSM6DSL_OUTY_H_XL       0x2B   // Accelerometer Y-axis High Register (MSB)
#define LSM6DSL_OUTZ_L_XL       0x2C   // Accelerometer Z-axis Low Register (LSB)
#define LSM6DSL_OUTZ_H_XL       0x2D   // Accelerometer Z-axis High Register (MSB)
//Accelerometer control register
void accel_raw();
//float calculateRunningAverage(int32_t *, int );
int Movement_Calibration(void);
int16_t readAccelerometerAxis(uint8_t regLow, uint8_t regHigh);
void LSM6DSL_EnableAccelerometer(void);
void IMU_Events(int16_t AcX, int16_t AcY, int16_t AcZ);

#endif /* INC_ACCELOMETER_H_ */
