#include "nvic.h"


void MultiNVIC_Init(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	// Encoder
	/*
	 * Group 0: 4bit Pree - 0 bit Sub
	 * Group 1: 3bit Pree - 1 bit Sub
	 * Group 2: 2bit Pree - 2 bit Sub
	 * Group 3: 1bit Pree - 3 bit Sub
	 * Group 4: 0bit Pree - 4 bit Sub
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	// EXTI
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	// UART1
//	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
	
	// Timer 2 Interrupt for IRSensor
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
//	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);
}