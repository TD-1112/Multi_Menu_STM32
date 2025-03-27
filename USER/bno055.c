/*
 * @file       MPU6050 library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/


#include "bno055.h"
#include "maths.h"
#include "i2c_2.h"
#include "delay.h"

BNO055_TypedefStruct BNO055;

static float gyro_error[3] = {0};
static float gyro_LSB = 131.0;
static float accel_LSB = 16384.0;

static uint32_t mpu_cur_time = 0;
static uint32_t mpu_pre_time = 0;

void BNO055_Init(void);
float BNO055_Get_Yaw(void);

__attribute__((constructor)) void Start_BNO055(void)
{
	BNO055.Init = BNO055_Init;
	BNO055.Get_Yaw = BNO055_Get_Yaw;
}


void BNO055_Init(void)
{
	I2C.Init();
	I2C.Write(BNO055_ADDRESS, BNO055_OPR_MODE, BNO055_MODE_NDOF); // Restart MPU6050
	delay_ms(20);
}

float BNO055_Get_Yaw(void)
{
	static float yaw = 0;
	
	uint8_t data[2] = {0};
	I2C.Read(BNO055_ADDRESS, BNO055_EUL_Heading_LSB, data, 2);
	
	yaw = data[0] | (data[1] << 8);
	
	return yaw / 16.0f;
}
