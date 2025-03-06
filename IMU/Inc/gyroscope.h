/*
 * gyroscope.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Shekhar Verma
 */

#ifndef INC_GYROSCOPE_H_
#define INC_GYROSCOPE_H_

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t gyroscopemegnato;
} GyroData;

#define LSM6DSL_OUTX_L_G       0x22   // GYRO X-axis Low Register (LSB)
#define LSM6DSL_OUTX_H_G       0x23   // GYRO X-axis High Register (MSB)
#define LSM6DSL_OUTY_L_G       0x24   // GYRO Y-axis Low Register (LSB)
#define LSM6DSL_OUTY_H_G       0x25   // GYRO Y-axis High Register (MSB)
#define LSM6DSL_OUTZ_L_G       0x26   // GYRO Z-axis Low Register (LSB)
#define LSM6DSL_OUTZ_H_G       0x27

void gyro_raw(void);
float calculateAngularAcceleration(void);
void detectHarshBraking(void);
int16_t readGyroscopeAxis(uint8_t , uint8_t );
void calculateRollPitchYaw(float,float,float,float,float*,float*,float*);
void LSM6DSL_Enablegyro(void);
#endif /* INC_GYROSCOPE_H_ */
