#include "define.h"

void Init(void);
void Run(void);

int main(void)
{
		Init();
    while(1)
    {
			Run();
    }
}

void Init(void)
{    
		Delay_Init();
		Button_Init();
    MPU6050.Init();
    USART1_Init(9600);
		Led_Debug();
    OLED_Init();  
    OLED_Clear(); 
    float calibrated_angle = 0;
}

void Run(void)
{
    uint16_t value1 = 1234;
    uint16_t value2 = 1231;
    uint16_t value3 = 1809;
    uint16_t value4 = 1123;
    uint16_t value5 = 1355;
    uint16_t value6 = 1780;
    float angle = MPU6050.Get_Yaw(0);
    USART1_Send_String("angle:");
    USART1_Send_Float(angle);
    USART1_Send_String("\n");
		GPIOC->ODR ^= GPIO_Pin_13;
    //Main_Menu(value1, value2, value3, value4, value5, value6,angle);
}