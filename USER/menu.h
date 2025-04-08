#ifndef __MENU__
#define __MENU__

#include "define.h"

// Timing configuration defines
#define DEBOUNCE_DELAY       50    // Button debounce time in milliseconds
#define LONG_PRESS_DURATION  500  // Long press detection time in milliseconds
#define UPDATE_INTERVAL      1   // Display update interval in milliseconds (changed from 0)
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

// Global sensor values
extern float g_angle;
extern uint16_t g_value_1;
extern uint16_t g_value_2;
extern uint16_t g_value_3;
extern uint16_t g_value_4;

// Main functions
void Check_Status(void);
void Reset_Status(void);
void Main_Menu(void);
void Select_Menu(uint8_t menu);

// Display update functions
void Update_Led(uint8_t show_calibrated, uint8_t led_cursor_pos);

// Helper functions for targeted screen clearing
void OLED_ClearLine(uint8_t line);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width);

// Button handling
uint8_t Check_Long_Press(uint8_t button_state, uint32_t current_time, 
                       uint32_t *button_press_start, uint8_t *is_button_held);

// Navigation functions - updated to not require sensor values
void Handle_LED_Navigation(uint8_t *led_cursor_pos, uint8_t *led_page);

// Calibration functions
void Handle_Calibration_Navigation(uint8_t *calib_cursor_pos, uint8_t max_pos);

// Gets updated sensor values and stores them in globals
void get_value(void);
#endif