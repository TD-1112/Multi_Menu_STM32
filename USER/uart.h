#ifndef __UART__
#define __UART__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"                  // Device header

typedef struct 
{
	void (*Init)(uint32_t baudrate);
	void (*SendNum)(float data);
	void (*SendNumLn)(float data);
	void (*SendString)(uint8_t *data);
	uint8_t (*ReceiveData)(uint8_t *data);
	
}UART_TypedefStruct;

extern UART_TypedefStruct UART;

#ifdef __cplusplus
}
#endif

#endif
