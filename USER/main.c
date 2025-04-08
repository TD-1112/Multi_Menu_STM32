#include "define.h"

void Init(void);

int main(void)
{

	Init();
	while(1)
	{
		on();
		Main_Menu();
	}
}

void Init(void)
{    
	Delay_Init();
	Button_Init();
	UART.Init(115200);
	Led_Debug();
	IRSensor_Init();
	Tim2IntForIRS_Init(5000, 72, 1);
	MPU6050.Init();
	OLED_Init();  
	OLED_Clear();
	off();
	MultiNVIC_Init();
	UART.SendString("OKE\n");
}

