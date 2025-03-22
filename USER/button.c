#include "button.h"

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitStructure.GPIO_Pin = BUTTON_1 | BUTTON_2 | BUTTON_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}

ButtonState Button_Read(uint16_t button)
{
    if(GPIO_ReadInputDataBit(BUTTON_PORT, button) == 0) {
        return PRESSED;
    } else {
        return RELEASED;
    }
}
