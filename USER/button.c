#include "button.h"

// Di chuyển từ menu.c sang button.c
flag_status flag_stt;
uint16_t stt_1;
uint16_t stt_2;
uint16_t stt_3;

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

// Di chuyển từ menu.c sang button.c
void Check_Status(void)
{
    static uint32_t last_debounce_time[3] = {0, 0, 0}; 
    static uint8_t button_state[3] = {1, 1, 1}; 
    static uint8_t last_reading[3] = {1, 1, 1}; 

    // Read the current state of each button
    uint8_t reading[3];
    reading[0] = Button_Read(BUTTON_1); // left button-Down button
    reading[1] = Button_Read(BUTTON_2); // right button-Up button
    reading[2] = Button_Read(BUTTON_3); // bot button - ok button

    // Update global variables for use in other functions
    stt_1 = reading[0];
    stt_2 = reading[1];
    stt_3 = reading[2];

    // Check each button for debouncing
    for (uint8_t i = 0; i < 3; i++) {
        // If the button state changed
        if (reading[i] != last_reading[i]) {
            // Reset the debouncing timer
            last_debounce_time[i] = millis();
        }
        
        // If enough time has passed since the last change
        if ((millis() - last_debounce_time[i]) > DEBOUNCE_DELAY) {
            // If the button state has changed
            if (reading[i] != button_state[i]) {
                button_state[i] = reading[i];
                
                // Only trigger on button press (when state goes from 1 to 0)
                if (button_state[i] == 0) {
                    // Set the appropriate flag
                    if (i == 0) flag_stt.flag_1 = 1;
                    if (i == 1) flag_stt.flag_2 = 1;
                    if (i == 2) flag_stt.flag_3 = 1;
                }
            }
        }
        
        // Save the reading for the next loop
        last_reading[i] = reading[i];
    }
}

void Reset_Status(void)
{
    flag_stt.flag_1 = 0;
    flag_stt.flag_2 = 0;
    flag_stt.flag_3 = 0;
}

uint8_t Check_Long_Press(uint8_t button_state, uint32_t current_time, 
                        uint32_t *button_press_start, uint8_t *is_button_held)
{
    if(button_state == 0) { // Button is pressed
        if(!(*is_button_held)) {
            // Button just pressed, record start time
            *button_press_start = current_time;
            *is_button_held = 1;
            return 0;
        } else {
            // Check if button has been held long enough
            uint32_t elapsed = current_time - *button_press_start;
            if(elapsed >= LONG_PRESS_DURATION) {
                return 1; // Long press detected
            }
        }
    } else {
        // Button released
        *is_button_held = 0;
    }
    
    return 0; // No long press detected yet
}
