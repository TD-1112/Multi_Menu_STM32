/*
 * @file       MPU6050 library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/


#ifndef __MPU6050__
#define __MPU6050__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

#define 	MPU6050_ADDRESS     						0x68
#define 	MPU6050_POWER_MANAGEMENT_REG    0x6B
#define 	MPU6050_GYRO_CONFIG_REG     		0x1B
#define 	MPU6050_ACCEL_CONFIG_REG    		0x1C
#define 	MPU6050_ACCEL_XOUT_H_REG        0x3B
#define 	MPU6050_ACCEL_XOUT_L_REG        0x3C
#define 	MPU6050_ACCEL_YOUT_H_REG        0x3D
#define 	MPU6050_ACCEL_YOUT_L_REG        0x3E
#define 	MPU6050_ACCEL_ZOUT_H_REG        0x3F
#define 	MPU6050_ACCEL_ZOUT_L_REG        0x40
#define 	MPU6050_TEMPERATURE_H_REG       0x41
#define 	MPU6050_TEMPERATURE_L_REG       0x42
#define 	MPU6050_GYRO_XOUT_H_REG         0x43
#define 	MPU6050_GYRO_XOUT_L_REG         0x44
#define 	MPU6050_GYRO_YOUT_H_REG         0x45
#define 	MPU6050_GYRO_YOUT_L_REG         0x46
#define 	MPU6050_GYRO_ZOUT_H_REG         0x47
#define 	MPU6050_GYRO_ZOUT_L_REG         0x48
	
#define 	MPU6050_FSC_GYRO_RANGE_250 			0x00
#define 	MPU6050_FSC_GYRO_RANGE_500 			0x08
#define 	MPU6050_FSC_GYRO_RANGE_1000 		0x10
#define 	MPU6050_FSC_GYRO_RANGE_2000 		0x18
	
#define 	MPU6050_FSC_ACCEL_RANGE_2 			0x00
#define 	MPU6050_FSC_ACCEL_RANGE_4 			0x08
#define 	MPU6050_FSC_ACCEL_RANGE_8 		  0x10
#define 	MPU6050_FSC_ACCEL_RANGE_16 		  0x18


typedef struct 
{
	void (*Init)(void);
	float *(*Get_AccelGyro)(void);
	void (*Set_FullScaleGyro)(uint8_t option);
	void (*Set_FullScaleAccel)(uint8_t option);
	float (*Get_Yaw)(int16_t value_reduction);
	
} MPU6050_TypedefStruct;

extern MPU6050_TypedefStruct MPU6050;

#ifdef __cplusplus
}
#endif
#endif