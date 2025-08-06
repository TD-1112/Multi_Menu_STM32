#ifndef __BUTTON__
#define __BUTTON__

#include "define.h"

#define BUTTON_PORT    GPIOB
#define BUTTON_1    GPIO_Pin_3
#define BUTTON_2    GPIO_Pin_4
#define BUTTON_3    GPIO_Pin_5

#define DEBOUNCE_DELAY       50    // Button debounce time in milliseconds
#define LONG_PRESS_DURATION  500   // Long press detection time in milliseconds

typedef enum {
    PRESSED = 0,
    RELEASED = 1,
} ButtonState;

typedef struct {
    uint8_t flag_1;
    uint8_t flag_2;
    uint8_t flag_3;
} flag_status;

extern flag_status flag_stt;
extern uint16_t stt_1;
extern uint16_t stt_2;
extern uint16_t stt_3;

void Button_Init(void);
ButtonState Button_Read(uint16_t button);
void Check_Status(void);
void Reset_Status(void);
uint8_t Check_Long_Press(uint8_t button_state, uint32_t current_time, 
                         uint32_t *button_press_start, uint8_t *is_button_held);

#endif

