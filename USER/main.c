#include "define.h"

void Init(void);
void Run(void);

int main(void)
{
   
	Init();
    while(1)
    {
			on();
			//IRSensor_PrintValue();
		//float raw_angle = MPU6050.Get_Yaw(0);
//		uint16_t value_1 = IRSensor_data[L_S]; // Lấy giá trị LED1
//		uint16_t value_2 =	IRSensor_data[FR_S] ; // Lấy giá trị LED2
//		uint16_t value_3 = IRSensor_data[FL_S]; // Lấy giá trị LED3
//		uint16_t value_4 = IRSensor_data[R_S]; // Lấy giá trị LED4
			uint16_t value_1 = 10;
			uint16_t value_2 = 10;
			uint16_t value_3 = 10;
			uint16_t value_4 = 10;
			Main_Menu();
    }
}

void Init(void)
{    
    Delay_Init();
    Button_Init();
    UART.Init(115200);

    Led_Debug();
	//IRSensor_Init();
	//Tim2IntForIRS_Init(5000, 72, 1);
	
		//BNO055.Init();
		
		MPU6050.Init();
    OLED_Init();  
    OLED_Clear();
		//USART1_Send_String("OKE\n");
	off();
	//MultiNVIC_Init();
}

