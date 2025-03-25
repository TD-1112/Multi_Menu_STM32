#include "define.h"

void Init(void);
void Run(void);

int main(void)
{
    Init();
    while(1)
    {
       float raw_angle = 90.15124;
       uint16_t value_1 = 1014; // Lấy giá trị LED1
       uint16_t value_2 = 2013; // Lấy giá trị LED2
       uint16_t value_3 = 4045; // Lấy giá trị LED3
       uint16_t value_4 = 6089; // Lấy giá trị LED4
//				float test = MPU6050.Get_Yaw(0);
//			off();
//			USART1_Send_Float(test);
//			USART1_Send_Char('\n');
//				OLED_ShowFloat(0, 0, test);
       Main_Menu(value_1, value_2, value_3, value_4, raw_angle);
			Debug();
    }
}

void Init(void)
{    
    Delay_Init();
    Button_Init();
    USART1_Init(9600);
    Led_Debug();
		//MPU6050.Init();
    OLED_Init();  
    OLED_Clear();
		USART1_Send_String("OKE\n");
		off();
}

