#ifndef __MENU__
#define __MENU__

#include "define.h"

// Timing configuration defines
#define DEBOUNCE_DELAY       50    // Button debounce time in milliseconds
#define LONG_PRESS_DURATION  1000  // Long press detection time in milliseconds
#define UPDATE_INTERVAL      100   // Display update interval in milliseconds
#define ANIMATION_INTERVAL   500   // Dot animation interval in milliseconds

// LED menu configuration
#define LED_MAX              6     // Total number of LEDs
#define LEDS_PER_PAGE        4     // Number of LEDs shown per page

typedef struct {
    uint8_t flag_1;
    uint8_t flag_2;
    uint8_t flag_3;
}flag_status;

extern flag_status flag_stat;

void check_status(void);
void reset_status(void);
void main_menu(void);
void select_menu(uint8_t menu);

#endif
