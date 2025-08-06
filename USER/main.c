#include "define.h"
#include "debug.h"

void Init(void);

int main(void)
{
	Init();
	Led_Debug();
	while(1)
	{
//		//Blink_Times(1);
//		if(Button_Read(GPIO_Pin_3) == PRESSED)
//			flag = 2 ;
//		Debug();
		Main_Menu();
	}
}

void Init(void)
{    
	Delay_Init();
	Button_Init();
	//UART.Init(115200);
//	IRSensor_Init();
//	Tim2IntForIRS_Init(5000, 72, 1);
//	MPU6050.Init();
	OLED_Init();  
	OLED_Clear();
	off();
//	MultiNVIC_Init();
//	UART.SendString("OKE\n");

}

