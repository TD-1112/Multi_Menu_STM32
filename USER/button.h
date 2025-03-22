#ifndef __BUTTON__
#define __BUTTON__


#include "define.h"

#define BUTTON_PORT    GPIOB
#define BUTTON_1    GPIO_Pin_3
#define BUTTON_2    GPIO_Pin_4
#define BUTTON_3    GPIO_Pin_5

typedef enum {
    PRESSED = 0,
    RELEASED = 1
} ButtonState;

void Button_Init(void);
ButtonState Button_Read(uint16_t button);

#endif

