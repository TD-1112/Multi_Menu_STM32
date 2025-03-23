#ifndef __MENU__
#define __MENU__

#include "define.h"

// Timing configuration defines
#define DEBOUNCE_DELAY       50    // Button debounce time in milliseconds
#define LONG_PRESS_DURATION  1000  // Long press detection time in milliseconds
#define UPDATE_INTERVAL      10   // Display update interval in milliseconds
#define ANIMATION_INTERVAL   500   // Dot animation interval in milliseconds

// LED menu configuration
#define LED_MAX              6     // Total number of LEDs
#define LEDS_PER_PAGE        4     // Number of LEDs shown per page

typedef struct {
    uint8_t flag_1;
    uint8_t flag_2;
    uint8_t flag_3;
}flag_status;

extern flag_status flag_stt;

extern float calibrated_angle;
extern uint16_t calibrated_leds[4];

// Main functions
void Check_Status(void);
void Reset_Status(void);
void Main_Menu(uint16_t value_1, uint16_t value_2, uint16_t value_3, 
               uint16_t value_4, uint16_t value_5, uint16_t value_6, float angle);
void Select_Menu(uint8_t menu, uint16_t value_1, uint16_t value_2, uint16_t value_3, 
                uint16_t value_4, uint16_t value_5, uint16_t value_6, float angle);

// Display update functions
void Update_Led(uint16_t value_1, uint16_t value_2, uint16_t value_3, uint16_t value_4, 
                uint16_t value_5, uint16_t value_6, uint8_t cursor_pos, uint8_t redraw_labels);
void Update_MPU(float value);

// Helper functions for targeted screen clearing
void OLED_ClearLine(uint8_t line);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width);

// Animation and UI effects
void Update_Animation_Dots(uint32_t current_time, uint32_t *last_animation_time, uint8_t *dot_state);

// Button handling
uint8_t Check_Long_Press(uint8_t button_state, uint32_t current_time, 
                        uint32_t *button_press_start, uint8_t *is_button_held);

// Navigation functions
void Handle_LED_Navigation(uint16_t value_1, uint16_t value_2, uint16_t value_3, 
                          uint16_t value_4, uint16_t value_5, uint16_t value_6, 
                          uint8_t *led_cursor_pos, uint8_t *led_page);

// Calibration functions
void Handle_Calibration_Navigation(uint8_t *calib_cursor_pos, uint8_t max_pos);


#endif
