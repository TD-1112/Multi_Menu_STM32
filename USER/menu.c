#include "menu.h"
#include "tim2.h"  // Make sure to include this for millis()

flag_status flag_stt;
uint16_t stt_1 ;
uint16_t stt_2 ;
uint16_t stt_3 ;

void check_status(void)
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

void reset_status(void)
{
    flag_stt.flag_1 = 0;
    flag_stt.flag_2 = 0;
    flag_stt.flag_3 = 0;
}

void main_menu(void)
{
    static uint8_t cursor_pos = 0;
    static uint8_t last_drawn_pos = 255; // Initialize to invalid position
    uint8_t menu_items = 4; // Changed from 3 to 4 to include all menu options
    
    // Handle button presses to update cursor_pos
    if(flag_stt.flag_1)
    {
        cursor_pos++;
        if(cursor_pos >= menu_items) 
            cursor_pos = 0;
        
        flag_stt.flag_1 = 0;
    }
    
    if(flag_stt.flag_2)
    {
        if(cursor_pos == 0)
            cursor_pos = menu_items - 1; 
        else
            cursor_pos--;
        
        flag_stt.flag_2 = 0;
    }
    
    // Only draw menu items once at initialization
    static uint8_t menu_initialized = 0;
    if(!menu_initialized) {
        OLED_ShowString(15, 0, "Calib MPU");
        OLED_ShowString(15, 1, "Calib Led");
        OLED_ShowString(15, 2, "Debug MPU");
        OLED_ShowString(15, 3, "Debug Led");
        menu_initialized = 1;
    }
    
    // Only redraw cursors if the position has changed
    if(cursor_pos != last_drawn_pos) {
        // Clear ALL possible cursor positions (0, 1, 2, 3)
        for(uint8_t i = 0; i < menu_items; i++) {
            OLED_ShowString(0, i, "  ");
        }
        
        // Draw cursor at new position
        OLED_ShowString(0, cursor_pos, "->");
        
        // Remember this position
        last_drawn_pos = cursor_pos;
    }
    
    if(flag_stt.flag_3)
    {
        // Call select_menu with the current cursor position
        select_menu(cursor_pos);
        
        // Force redraw of menu when we return
        menu_initialized = 0; 
        last_drawn_pos = 255;
        
        flag_stt.flag_3 = 0;
    }
}


void select_menu(uint8_t menu)
{
    uint32_t button_press_start = 0;
    uint8_t is_button_held = 0;
    
    // For updating real-time values in debug screens
    uint32_t last_update_time = 0;
    
    // Animation variables for calibration screens
    uint8_t dot_state = 0; 
    uint32_t last_animation_time = 0;
    
    // Auto-exit timer for calibration menus
    uint32_t calib_start_time = millis();
    
    // Visual feedback variables
    uint8_t progress_indicator = 0;
    
    // LED debug menu variables
    uint8_t led_cursor_pos = 0;       // Position 0-5 (absolute)
    uint8_t led_page = 0;             // Current page (0 or 1)
    
    // Clear screen and display the selected menu
    OLED_Clear();
    
    switch(menu) {
        case 0:
            // Handle Calib MPU selection
            OLED_ShowString(20, 0, "Calib MPU");
            OLED_ShowString(0, 2, "Please wait");
            break;
        case 1:
            OLED_ShowString(20, 0, "Calib Led");
            OLED_ShowString(0, 2, "Please wait");
            break;
        case 2:
            OLED_ShowString(20, 0, "Debug MPU");
            OLED_ShowString(0, 1, "Accel: 0.1g" );
            OLED_ShowString(0, 2, "Gyro: 0.1 deg/s");
            break;
        case 3:
            // Draw first page of LEDs (LEDs 1-4)
            OLED_ShowString(15, 0, "LED1: OFF");
            OLED_ShowString(15, 1, "LED2: OFF");
            OLED_ShowString(15, 2, "LED3: OFF");
            OLED_ShowString(15, 3, "LED4: OFF");
            OLED_ShowString(0, 0, "->"); // Initial cursor position
            break;
    }
    
    reset_status();
    
    // Wait in this menu until button 3 is held for 1 second or auto-exit timer expires
    while(1) {
        check_status();
        uint32_t current_time = millis();
        
        // Auto-exit for calibration menus (cases 0 and 1)
        if((menu == 0 || menu == 1) && (current_time - calib_start_time >= CALIB_AUTO_EXIT_TIME)) {
            // Auto-exit after timer expires
            break;
        }
        
        // LED menu navigation handling
        if(menu == 3) {
            // Handle Down button (flag_stt.flag_1)
            if(flag_stt.flag_1) {
                // Clear current cursor (based on page and position)
                uint8_t display_row = led_cursor_pos - (led_page * LEDS_PER_PAGE);
                OLED_ShowString(0, display_row, "  ");
                
                // Move cursor down
                led_cursor_pos++;
                if(led_cursor_pos >= LED_MAX) {
                    led_cursor_pos = 0; // Wrap to top
                }
                
                // Check if we need to change page
                uint8_t new_page = led_cursor_pos / LEDS_PER_PAGE;
                if(new_page != led_page) {
                    led_page = new_page;
                    OLED_Clear();
                    
                    if(led_page == 0) {
                        // Show LEDs 1-4
                        OLED_ShowString(15, 0, "LED1: OFF");
                        OLED_ShowString(15, 1, "LED2: OFF");
                        OLED_ShowString(15, 2, "LED3: OFF");
                        OLED_ShowString(15, 3, "LED4: OFF");
                    } else {
                        // Show LEDs 5-6
                        OLED_ShowString(15, 0, "LED5: OFF");
                        OLED_ShowString(15, 1, "LED6: OFF");
                    }
                }
                
                // Draw cursor at new position
                display_row = led_cursor_pos - (led_page * LEDS_PER_PAGE);
                OLED_ShowString(0, display_row, "->");
                
                flag_stt.flag_1 = 0;
            }
            
            // Handle Up button (flag_stt.flag_2)
            if(flag_stt.flag_2) {
                // Clear current cursor
                uint8_t display_row = led_cursor_pos - (led_page * LEDS_PER_PAGE);
                OLED_ShowString(0, display_row, "  ");
                
                // Move cursor up
                if(led_cursor_pos == 0) {
                    led_cursor_pos = LED_MAX - 1; // Wrap to bottom
                } else {
                    led_cursor_pos--;
                }
                
                // Check if we need to change page
                uint8_t new_page = led_cursor_pos / LEDS_PER_PAGE;
                if(new_page != led_page) {
                    led_page = new_page;
                    OLED_Clear();
                    
                    if(led_page == 0) {
                        // Show LEDs 1-4
                        OLED_ShowString(15, 0, "LED1: OFF");
                        OLED_ShowString(15, 1, "LED2: OFF");
                        OLED_ShowString(15, 2, "LED3: OFF");
                        OLED_ShowString(15, 3, "LED4: OFF");
                    } else {
                        // Show LEDs 5-6
                        OLED_ShowString(15, 0, "LED5: OFF");
                        OLED_ShowString(15, 1, "LED6: OFF");
                    }
                }
                
                // Draw cursor at new position
                display_row = led_cursor_pos - (led_page * LEDS_PER_PAGE);
                OLED_ShowString(0, display_row, "->");
                
                flag_stt.flag_2 = 0;
            }
        }
        
        // Long press detection for button 3 - simplified
        if(stt_3 == 0) {
            if(!is_button_held) {
                // Button just pressed, record start time
                button_press_start = current_time;
                is_button_held = 1;
            } else {
                // Check if button has been held long enough
                uint32_t elapsed = current_time - button_press_start;
                if(elapsed >= LONG_PRESS_DURATION) {
                    // Just exit without any visual indicator
                    break;
                }
                // Remove all progress indicator code
            }
        } else {
            // Button released
            is_button_held = 0;
            // No need to clear any indicators
        }
        
        // Update real-time data periodically
        if(current_time - last_update_time >= UPDATE_INTERVAL) {
            last_update_time = current_time;
            
            // Update data based on which menu we're in
            if(menu == 2) { // Debug MPU
                // Update MPU debug data here
            } 
            else if(menu == 3) { // Debug LED
                // Update LED status here - use actual LED states
                // Example: 
                // if(led_page == 0) {
                //     OLED_ShowString(70, 1, LED1_State ? "ON " : "OFF");
                //     OLED_ShowString(70, 2, LED2_State ? "ON " : "OFF");
                //     // etc.
                // }
            }
        }
        
        // Update animation for calibration screens
        if((menu == 0 || menu == 1) && (current_time - last_animation_time >= ANIMATION_INTERVAL)) {
            last_animation_time = current_time;
            
            // Clear the dots area first
            OLED_ShowString(100, 2, "    ");
            
            // Display dots based on current state
            if(dot_state == 1) {
                OLED_ShowString(90, 2, ".");
            } else if(dot_state == 2) {
                OLED_ShowString(90, 2, "..");
            } else if(dot_state == 3) {
                OLED_ShowString(90, 2, "...");
            }
            // dot_state 0 means no dots (already cleared)
            
            // Move to next state
            dot_state = (dot_state + 1) % 4; // Cycle through 0,1,2,3
            
            // Optional: you could add a countdown indicator here
            // For example: show "5...", "4...", etc.
            uint8_t seconds_left = (CALIB_AUTO_EXIT_TIME - (current_time - calib_start_time)) / 1000 + 1;
            char countdown[5];
            sprintf(countdown, "%ds", seconds_left);
            OLED_ShowString(0, 4, countdown);
        }
    }
    
    // Handle transition back to main menu
    OLED_Clear();
    reset_status();
}