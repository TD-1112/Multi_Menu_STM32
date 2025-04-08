#ifndef __DEFINE__
#define __DEFINE__


#include "stm32f10x.h"
#include "delay.h"
#include "i2c.h"
#include "button.h"
#include "oled.h"
#include "uart.h"
#include "debug.h"
#include "menu.h"
#include "math.h"
#include "bno055.h"
#include "i2c_2.h"
#include "irsensor.h"
#include "nvic.h"
#include "MPU6050.h"

#define CELL   180

// Motor
/*
TIMER		| 	L1		-		L2		-		R1		-		R2
TIM1		| 	PA8		|		PA9		|		PA10	|		PA11	
*/

#define PERIOD							5000-1
#define PRESCALER						72-1

// Encoder (co dinh - khong the thay doi)
#define ENA_LEFT_PIN				GPIO_Pin_0
#define ENB_LEFT_PIN				GPIO_Pin_1
#define ENA_RIGHT_PIN				GPIO_Pin_2
#define ENB_RIGHT_PIN				GPIO_Pin_3
#define EN_PORT							GPIOA

// MPU6050
#define MPU6050_I2C  			I2C2
#define MPU6050_CLOCK			100000

// Receive led
#define REC_LED_RF       	GPIO_Pin_4  // GPIOA    //ADC_Channel_4
#define REC_LED_LF       	GPIO_Pin_5  // GPIOA    //ADC_Channel_5
#define REC_LED_DL       	GPIO_Pin_6  // GPIOA    //ADC_Channel_6
#define REC_LED_L       	GPIO_Pin_7  // GPIOA    //ADC_Channel_7
#define REC_LED_DR       	GPIO_Pin_0  // GPIOB    //ADC_Channel_8
#define REC_LED_R       	GPIO_Pin_1  // GPIOB    //ADC_Channel_9

// Transmitter led
#define TRAN_LED_PORT     GPIOB 
#define TRAN_LED_S       	GPIO_Pin_12
#define TRAN_LED_F       	GPIO_Pin_13
#define TRAN_LED_DR       GPIO_Pin_14
#define TRAN_LED_DL       GPIO_Pin_15
#define ALL_TRAN_LEDS     TRAN_LED_F | TRAN_LED_S | TRAN_LED_DL | TRAN_LED_DR
#endif