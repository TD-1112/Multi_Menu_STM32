#include "stm32f10x.h"
#include "oled.h"
#include "uart.h"
#include "tim2.h"
#include "i2c.h"
#include "button.h"

void Led_Debug(void);
void Debug(void);
void on(void);
void off(void);
void Blink_Times(uint8_t times);
uint8_t flag = 0;

int main(void)
{
    Timer2_Init();
    Delay_ms(100);
		Button_Init();
    OLED_Init();  
    OLED_Clear(); 
    OLED_ShowString(0,0, "hiiii");
    OLED_Scroll();
    Led_Debug();
	off();
    while(1)
    {
        uint16_t value1 = Button_Read(BUTTON_1);
        uint16_t value2 = Button_Read(BUTTON_2);
        uint16_t value3 = Button_Read(BUTTON_3);
        
						if(value1 == 0) flag = 1;
        else if(value2 == 0) flag = 2;
        else if(value3 == 0) flag = 3;
        else flag = 0;
        
        Debug();
        //Delay_ms(100); // Debounce delay
    }
}

void Led_Debug(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void on(void)
{    
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void Blink_Times(uint8_t times)
{
    for(uint8_t i = 0; i < times; i++)
    {
        on();
        Delay_ms(500);
        off();
        Delay_ms(500);
    }
}

void Debug(void)
{
    switch(flag)
    {
        case 1:
            Blink_Times(1);
            break;
        case 2:
            Blink_Times(2);
            break;
        case 3:
            Blink_Times(3);
            break;
        default:
            off();
            break;
    }
    flag = 0; // Reset flag after executing pattern
}