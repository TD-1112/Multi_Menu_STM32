#include "irsensor.h"
#include "delay.h"
#include "uart.h"

volatile uint16_t IRSensor_data[6] = {0};
volatile uint16_t temp_data[6] = {0};
volatile uint8_t index = 0;


void IRSensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	DMA_InitTypeDef		DMA_InitStruct;
	
	// Config clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	// Receive led
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = REC_LED_RF | REC_LED_LF | REC_LED_DL | REC_LED_L;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = REC_LED_DR | REC_LED_R;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// Transmitter led
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = TRAN_LED_F | TRAN_LED_S | TRAN_LED_DL | TRAN_LED_DR;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TRAN_LED_PORT, &GPIO_InitStruct);
	
	// DMA
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)temp_data;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = 6;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	
	// ADC
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_NbrOfChannel = 6;
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
}

void Tim2IntForIRS_Init(uint16_t period, uint16_t prescaler, uint8_t status)
{
	TIM_TimeBaseInitTypeDef			TIM2_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, status);
	
	TIM2_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM2_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM2_InitStruct.TIM_Period = period -1;
	TIM2_InitStruct.TIM_Prescaler = prescaler -1;
	
	TIM_TimeBaseInit(TIM2, &TIM2_InitStruct);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, status);
	TIM_Cmd(TIM2, status);
}


//uint16_t IRSensor_ReadOneSensor(uint8_t chnl)
//{
//	uint16_t value = 0;
//	uint8_t channel = chnl;
//	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
////	for(char i=0; i<10; i++)
//	{
//		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
//		value += ADC_GetConversionValue(ADC1);
//	}
//	return value;
//}

//void IRSensor_ReadAllSensors(void)
//{
//	IRSensor_data[2] = IRSensor_ReadOneSensor(LED_CHANNEL_RF);
//	IRSensor_data[3] = IRSensor_ReadOneSensor(LED_CHANNEL_LF);
//	IRSensor_data[0] = IRSensor_ReadOneSensor(LED_CHANNEL_R);
//	IRSensor_data[5] = IRSensor_ReadOneSensor(LED_CHANNEL_L);
//	IRSensor_data[1] = IRSensor_ReadOneSensor(LED_CHANNEL_DR);
//	IRSensor_data[4] = IRSensor_ReadOneSensor(LED_CHANNEL_DL);
//}

void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	switch(index)
	{
		case 0:
			IRSensor_data[FR_S] = temp_data[FR_S];
			IRSensor_data[FL_S] = temp_data[FL_S];
			IRSensor_EndTransmitter(ALL_TRAN_LEDS);
			IRSensor_StartTransmitter(TRAN_LED_DR | TRAN_LED_DL);
			break;
		
		case 1:
			IRSensor_data[DR_S] = temp_data[DR_S];
			IRSensor_data[DL_S] = temp_data[DL_S];
			IRSensor_EndTransmitter(ALL_TRAN_LEDS);
			IRSensor_StartTransmitter(TRAN_LED_F);
			break;
		
		case 2:
			IRSensor_data[R_S] = temp_data[R_S];
			IRSensor_data[L_S] = temp_data[L_S];
			IRSensor_EndTransmitter(ALL_TRAN_LEDS);
			IRSensor_StartTransmitter(TRAN_LED_S);
			break;
	}
	index = index > 2 ? 0 : index +1;
}

void IRSensor_PrintValue(void)
{
	UART.SendString("FR ");
	UART.SendNum(IRSensor_data[FR_S]);
	UART.SendString(" DR ");
	UART.SendNum(IRSensor_data[DR_S]);
	UART.SendString(" R ");
	UART.SendNum(IRSensor_data[R_S]);
	UART.SendString(" L ");
	UART.SendNum(IRSensor_data[L_S]);
	UART.SendString(" DL ");
	UART.SendNum(IRSensor_data[DL_S]);
	UART.SendString(" FL ");
	UART.SendNum(IRSensor_data[FL_S]);
	UART.SendString(" - \n");
}
