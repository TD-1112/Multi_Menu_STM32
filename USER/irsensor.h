#ifndef __IRSENSOR__
#define __IRSENSOR__
#ifdef __cplusplus
    extern "C" {
#endif
#include "stm32f10x.h"                  // Device header

#include "define.h"

#define 	FR_S			5
#define 	FL_S			3
#define 	DR_S			4
#define 	DL_S			2
#define 	R_S				0
#define 	L_S				1
			
#define  IRSensor_StartTransmitter(GPIO_Pin)	TRAN_LED_PORT->ODR |= GPIO_Pin;

#define  IRSensor_EndTransmitter(GPIO_Pin)		TRAN_LED_PORT->ODR &= ~GPIO_Pin;

typedef enum
{
	LED_CHANNEL_RF = 0x04,
	LED_CHANNEL_LF,
	LED_CHANNEL_DL,
	LED_CHANNEL_L,
	LED_CHANNEL_DR,
	LED_CHANNEL_R,
	
}ledChannel_E;

extern volatile uint16_t IRSensor_data[6];

void IRSensor_Init(void);
void Tim2IntForIRS_Init(uint16_t period, uint16_t prescaler, uint8_t status);
uint16_t IRSensor_ReadOneSensor(uint8_t chnl);
void IRSensor_ReadAllSensors(void);
void IRSensor_PrintValue(void);

#ifdef __cplusplus
    }
#endif
#endif
