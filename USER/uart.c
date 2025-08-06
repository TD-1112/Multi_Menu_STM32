#include "uart.h"
#include "string.h"
#include "stdio.h"

volatile uint8_t UARTx_dataReceived[100] = "0\0";
volatile uint8_t rx_flag = 0;
volatile uint8_t id = 0;
volatile uint8_t timeout = 0;

void UART1_init(uint32_t baudrate);
void UART1_SendNum(float data);
void UART1_SendNumLn(float data);
void UART1_SendString(uint8_t *data);
uint8_t UART1_ReceiveData(uint8_t *data);


UART_TypedefStruct UART;

void UART_FirstInit(void) __attribute__ ((constructor));			// Ham nay se duoc chay truoc khi vao ham main

void UART_FirstInit(void)
{
	UART.Init = UART1_init;
	UART.ReceiveData = UART1_ReceiveData;
	UART.SendNum = UART1_SendNum;
	UART.SendNumLn = UART1_SendNumLn;
	UART.SendString = UART1_SendString;
}

void UART1_init(uint32_t baudrate)
{
	GPIO_InitTypeDef 		GPIO_s;
	USART_InitTypeDef 	UART_s;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	GPIO_s.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_s.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_s.GPIO_Pin   = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_s);
	
	GPIO_s.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_s.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_s.GPIO_Pin   = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_s);
	
	UART_s.USART_BaudRate = baudrate;											/* toc do truyen: baud_rates */
	UART_s.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	UART_s.USART_Parity = USART_Parity_No;
	UART_s.USART_StopBits = USART_StopBits_1;
	UART_s.USART_WordLength = USART_WordLength_8b;
	UART_s.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&UART_s);
	USART_Cmd(USART1,ENABLE);																/* cho phep USART1 hoat dong */
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);						/* ngat nhan USART1 					*/
}

uint8_t UART1_ReceiveData(uint8_t *data)
{
	uint8_t new_data = 0;
	uint8_t index = 0;
	if(rx_flag == 0) timeout = 0;
	if(rx_flag && timeout < 1) {
		volatile uint16_t i;
		for(i = 0; i< 0xffff; i++);
		
		timeout++;
	}
	else if(rx_flag == 1 && timeout >= 1)
	{
		UARTx_dataReceived[id++] = '\0';
		id = 0;
		rx_flag = 0;
		new_data = 1;
		do{ 
			data[index] = UARTx_dataReceived[index]; 
		}while(UARTx_dataReceived[index++]);
	}
	return new_data;
}

void UART1_SendString(uint8_t *data)
{
	do{
		USART_SendData(USART1, *data);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}while(*(++data));
}

void UART1_SendNum(float data)
{
	uint8_t Number[50];
	sprintf(Number,"%.3f",data);
	UART1_SendString(Number);
}

void UART1_SendNumLn(float data)
{
	uint8_t Number[50];
	sprintf(Number,"%.3f\n",data);
	UART1_SendString(Number);
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		timeout = 0;
		if(rx_flag == 0){
			rx_flag = 1;
		}
		UARTx_dataReceived[id++] = USART1->DR;
	}
}