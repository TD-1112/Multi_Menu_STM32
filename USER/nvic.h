/*
 * @file       Motor library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/

#ifndef __NVIC__
#define __NVIC__
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

void MultiNVIC_Init(void);

#ifdef __cplusplus
}
#endif
#endif