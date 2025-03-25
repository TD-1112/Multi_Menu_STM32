#include "define.h"

void Init(void);
void Run(void);

int main(void)
{
    Init();

    
    while(1)
    {
        float raw_angle = 90.15124;
        uint16_t value_1 = 10; // Lấy giá trị LED1
        uint16_t value_2 = 20; // Lấy giá trị LED2
        uint16_t value_3 = 40; // Lấy giá trị LED3
        uint16_t value_4 = 60; // Lấy giá trị LED4
        //OLED_ShowFloat(0, 0, raw_angle);
        //off();
        Main_Menu(value_1, value_2, value_3, value_4, raw_angle);
        // on();
    }
}

void Init(void)
{    
    Delay_Init();
    Button_Init();
    USART1_Init(9600);
    Led_Debug();
    OLED_Init();  
    OLED_Clear(); 
}

