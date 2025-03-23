#ifndef __MENU__
#define __MENU__

#include "define.h"

// Timing configuration defines
#define DEBOUNCE_DELAY       50    // Button debounce time in milliseconds
#define LONG_PRESS_DURATION  1000  // Long press detection time in milliseconds
#define UPDATE_INTERVAL      100   // Display update interval in milliseconds
#define ANIMATION_INTERVAL   500   // Dot animation interval in milliseconds
#define CALIB_AUTO_EXIT_TIME 5000  // Calibration auto-exit time (5 seconds)

// LED menu configuration
#define LED_MAX              6     // Total number of LEDs
#define LEDS_PER_PAGE        4     // Number of LEDs shown per page

typedef struct {
    uint8_t flag_1;
    uint8_t flag_2;
    uint8_t flag_3;
}flag_status;

extern flag_status flag_stt;

void Check_Status(void);
void Reset_Status(void);
void Main_Menu( uint16_t value_1 , uint16_t value_2 , uint16_t value_3 , 
                uint16_t value_4 , uint16_t value_5 , uint16_t value_6);
//void Main_Menu(uint16_t value_1 , uint16_t value_2 , uint16_t value_3 , uint16_t value_4 , uint16_t value_5 , uint16_t value_6);
void Select_Menu(uint8_t menu, uint16_t value_1, uint16_t value_2, uint16_t value_3, 
    uint16_t value_4, uint16_t value_5, uint16_t value_6);
void Update_Led(uint16_t value_1, uint16_t value_2, uint16_t value_3, uint16_t value_4, 
                uint16_t value_5, uint16_t value_6, uint8_t cursor_pos, uint8_t redraw_labels);
void Update_MPU(float accel);

#endif
