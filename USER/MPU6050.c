/*
 * @file       MPU6050 library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/
#include "mpu6050.h"
#include "maths.h"
#include "i2c_2.h"
#include "delay.h"

MPU6050_TypedefStruct MPU6050;

static float gyro_error[3] = {0};
static float gyro_LSB = 131.0;
static float accel_LSB = 16384.0;

static uint32_t mpu_cur_time = 0;
static uint32_t mpu_pre_time = 0;

void MPU6050_Init(void);
float *MPU6050_Get_AccelGyro(void);
float MPU6050_Get_Yaw(int16_t value_reduction);
void MPU6050_Set_FullScaleGyro(uint8_t option);
void MPU6050_Set_FullScaleAccel(uint8_t option);
void MPU6050_Set_GyroError(void);

void Start_MPU6050(void)__attribute__((constructor));
void Start_MPU6050(void)
{
	MPU6050.Init = MPU6050_Init;
	MPU6050.Get_AccelGyro = MPU6050_Get_AccelGyro;
	MPU6050.Set_FullScaleGyro = MPU6050_Set_FullScaleGyro;
	MPU6050.Set_FullScaleAccel = MPU6050_Set_FullScaleAccel;
	MPU6050.Get_Yaw = MPU6050_Get_Yaw;
}


void MPU6050_Init(void)
{
	I2C.Init();
	uint8_t data = 0x80;
	I2C.Write(MPU6050_ADDRESS, MPU6050_POWER_MANAGEMENT_REG, data); // Restart MPU6050
	delay_ms(200);
	data = 0x00;
	I2C.Write(MPU6050_ADDRESS, MPU6050_POWER_MANAGEMENT_REG, data); // Thoat khoi che do sleep -> 0x6B = 0
	
	MPU6050.Set_FullScaleGyro(MPU6050_FSC_GYRO_RANGE_2000);
	MPU6050.Set_FullScaleAccel(MPU6050_FSC_ACCEL_RANGE_16);
	
	delay_ms(1000);
	MPU6050_Set_GyroError();
}

float *MPU6050_Get_AccelGyro(void)
{
	static float accelgyro[6] = {0};

	mpu_cur_time = micros();
	if(mpu_cur_time - mpu_pre_time >= 1)
	{
		uint8_t data[14] = {0};
		uint32_t dt = mpu_cur_time - mpu_pre_time;
		
		mpu_pre_time = mpu_cur_time;
		
		I2C.Read(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H_REG, data, 14);
		
		for(uint8_t i = 0; i<3; i++)
		{
			accelgyro[i] = (float)((int16_t)(((uint16_t)data[2*i]<<8) | (uint16_t)data[2*i+1])) / accel_LSB;
		}
		
		for(uint8_t i = 3; i<6; i++)
		{
			accelgyro[i] -= ((float)((int16_t)(((uint16_t)data[2*i+2]<<8) | (uint16_t)data[2*i+3])) - gyro_error[i-3]) * dt / 1000000.0 / gyro_LSB;
		}
	}
	return accelgyro;
}

float MPU6050_Get_Yaw(int16_t value_reduction)
{
	static float yaw = 0;
	static float pre_yaw = 0;
	
	yaw -= value_reduction;

	mpu_cur_time = micros();
	if(mpu_cur_time - mpu_pre_time >= 1)
	{
		uint8_t data[2] = {0};
		uint32_t dt = mpu_cur_time - mpu_pre_time;
		if(dt > 10000) dt = 10000;
		mpu_pre_time = mpu_cur_time;
		
		I2C.Read(MPU6050_ADDRESS, MPU6050_GYRO_ZOUT_H_REG, data, 2);
		
		yaw -= ((float)((int16_t)(((uint16_t)data[0]<<8) | (uint16_t)data[1])) - gyro_error[2]) * dt / 1000000.0 / gyro_LSB;
	}
	// calib yaw
	if(value_reduction)
	{
		pre_yaw = yaw;
	}
	else if(Abs(yaw - pre_yaw) >= 1)  // chieu duong
	{
		yaw += 9198.80683 * (yaw - pre_yaw) / 1000000UL;
		pre_yaw = yaw;
	}
	return yaw;
}

void MPU6050_Set_FullScaleGyro(uint8_t option)
{
	I2C.Write(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG_REG, option);
	gyro_LSB = 131.0 / (float)(1 << (option >> 3));
}

void MPU6050_Set_FullScaleAccel(uint8_t option)
{
	I2C.Write(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG_REG, option);
	accel_LSB = 16384.0 / (float)(1 << (option >> 3));
}

void MPU6050_Set_GyroError(void)
{
	uint8_t data[14] = {0};
	int32_t gyro_sum[3] = {0};
	
	for(uint16_t n=0; n<1000; n++)
	{
		I2C.Read(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H_REG, data, 14);
		for(uint8_t i = 0; i<3; i++)
		{
			gyro_sum[i] += (int16_t)(((uint16_t)data[2*i+8]<<8) | (uint16_t)data[2*i+9]);
		}
	}
	
	for(uint8_t i = 0; i<3; i++)
	{
		gyro_error[i] = (float)gyro_sum[i] / 1000;
	}
}
