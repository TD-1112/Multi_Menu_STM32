/*
 * @file       I2C library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/


#ifndef __I2C_2__
#define __I2C_2__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

typedef struct 
{
	void (*Init)(void);
	void (*Write)(uint8_t address, uint8_t reg, uint8_t data);
	void (*Read)(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len);
	
} I2C_TypedefStruct;

extern I2C_TypedefStruct I2C;

#ifdef __cplusplus
}
#endif
#endif