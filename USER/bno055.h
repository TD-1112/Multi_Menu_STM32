/*
 * @file       MPU6050 library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/


#ifndef __BNO055__
#define __BNO055__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

#define BNO055_ADDRESS 0x28  // Ho?c 0x29 n?u pin ADR n?i VCC
#define BNO055_OPR_MODE 0x3D
#define BNO055_EUL_Heading_LSB 0x1A
#define BNO055_MODE_NDOF 0x0C

typedef struct 
{
	void (*Init)(void);
	float (*Get_Yaw)(void);
	
} BNO055_TypedefStruct;

extern BNO055_TypedefStruct BNO055;

#ifdef __cplusplus
}
#endif
#endif