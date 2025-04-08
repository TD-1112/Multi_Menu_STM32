#include "menu.h"

flag_status flag_stt;

uint16_t stt_1;
uint16_t stt_2;
uint16_t stt_3;

float calibrated_angle = 0;
uint16_t calibrated_leds[4];
static uint8_t first_entry = 1;  // Thêm biến để kiểm tra lần đầu vào menu

// Add these global variables near the top of the file, with other globals
float g_angle;
uint16_t g_value_1;
uint16_t g_value_2;
uint16_t g_value_3;
uint16_t g_value_4;

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

void Main_Menu(void)   
{
    Check_Status();
    static uint8_t cursor_pos = 0;
    static uint8_t last_drawn_pos = 255; // Initialize to invalid position
    uint8_t menu_items = 4; // Changed from 4 to 3 (removed Debug MPU)
    
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
        OLED_ShowString(20, 0, "Calib MPU");
        OLED_ShowString(20, 1, "Calib Led");
        OLED_ShowString(20, 2, "Debug Led");
				OLED_ShowString(20, 3, "Exit");
        menu_initialized = 1;
    }
    
    // Only redraw cursors if the position has changed
    if(cursor_pos != last_drawn_pos) {
        // Clear ALL possible cursor positions (0, 1, 2,3)
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
        // Update sensor values before entering submenu
        get_value();
        
        // Call Select_Menu with the current cursor position and global variables
        Select_Menu(cursor_pos);
        
        // Force redraw of menu when we return
        menu_initialized = 0; 
        last_drawn_pos = 255;
        
        flag_stt.flag_3 = 0;
    }
}

// New long press detection function
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

// New LED navigation function
void Handle_LED_Navigation(uint8_t *led_cursor_pos, uint8_t *led_page)
{
    // Handle Down button (flag_stt.flag_1)
    if(flag_stt.flag_1) {
        // Move cursor down
        (*led_cursor_pos)++;
        if(*led_cursor_pos >= LED_MAX) {
            *led_cursor_pos = 0; // Wrap to top
        }
        
        // Check if we need to change page
        uint8_t new_page = *led_cursor_pos / LEDS_PER_PAGE;
        if(new_page != *led_page) {
            *led_page = new_page;
            OLED_Clear(); // Still need to clear when changing pages
        }
        
        // Update display with new cursor position
        Update_Led(0, *led_cursor_pos);
        flag_stt.flag_1 = 0;
    }
    
    // Handle Up button (flag_stt.flag_2)
    if(flag_stt.flag_2) {
        // Move cursor up
        if(*led_cursor_pos == 0) {
            *led_cursor_pos = LED_MAX - 1; // Wrap to bottom
        } else {
            (*led_cursor_pos)--;
        }
        
        // Check if we need to change page
        uint8_t new_page = *led_cursor_pos / LEDS_PER_PAGE;
        if(new_page != *led_page) {
            *led_page = new_page;
            OLED_Clear(); // Still need to clear when changing pages
        }
        
        // Update display with new cursor position
        Update_Led(0, *led_cursor_pos);
        flag_stt.flag_2 = 0;
    }
    
    // Handle toggle button (flag_stt.flag_3)
    if(flag_stt.flag_3) {
        // Signal which LED should be toggled
        // Replace this with your actual LED toggle logic
        // LED_Toggle(*led_cursor_pos + 1);  
        
        flag_stt.flag_3 = 0;
    }
}

// Add these helper functions for targeted screen clearing
void OLED_ClearLine(uint8_t line)
{
    OLED_ShowString(0, line, "                    "); // 20 spaces to clear line
}

void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width)
{
    char spaces[21] = "                    "; // 20 spaces
    spaces[width] = '\0'; // Truncate to desired width
    OLED_ShowString(x, y, spaces);
}

// Updated calibration functions for better organization
void Handle_Calibration_Navigation(uint8_t *calib_cursor_pos, uint8_t max_pos)
{
    // Handle Down button (flag_stt.flag_1)
    if(flag_stt.flag_1) {
        (*calib_cursor_pos)++;
        if(*calib_cursor_pos >= max_pos) {
            *calib_cursor_pos = 0; // Wrap around
        }
        flag_stt.flag_1 = 0;
    }
    if(flag_stt.flag_2) {
        if(*calib_cursor_pos == 0) {
            *calib_cursor_pos = max_pos - 1; // Wrap around
        } else {
            (*calib_cursor_pos)--;
        }
        flag_stt.flag_2 = 0;
    }
}

// Updated Select_Menu function for calibration
void Select_Menu(uint8_t menu)
{
    uint32_t button_press_start = 0;
    uint8_t is_button_held = 0;
    
    // For updating real-time values in debug screens
    uint32_t last_update_time = 0;
    
    // LED debug menu variables
    uint8_t led_cursor_pos = 0;       // Position 0-5 (absolute)
    uint8_t led_page = 0;             // Current page (0 or 1)
    
    // Calibration menu variables
    uint8_t calib_cursor_pos = 0;     // Position for yes/no selection or values screen
    uint8_t calib_page = 0;           // For LED calibration (0=values, 1=yes/no)
    uint8_t last_calib_cursor_pos = 255; // For efficient redraw
    
    // Clear screen and display the selected menu
    OLED_Clear();
    
    switch(menu) {
        case 0: // Calib MPU
            get_value();
            OLED_ShowString(25, 0, "Angle: ");
            OLED_ShowFloat(75, 0, g_angle);
            OLED_ShowString(25, 1, "Saved:");
            OLED_ShowFloat(75, 1, calibrated_angle);
            OLED_ShowString(25, 2, "Exit");
            OLED_ShowString(0, 0, "->");  // Initial cursor position on Angle
            break;
            
        case 1: // Calib LED
            if(calib_page == 0) {
                //value LED current
                OLED_ShowString(0, 0, "FR: ");
                OLED_ShowNum(40, 0, g_value_1);
                OLED_ShowString(0, 2, "R: ");
                OLED_ShowNum(40, 2, g_value_2);
                OLED_ShowString(0, 3, "L: ");
                OLED_ShowNum(40, 3, g_value_3);
                OLED_ShowString(0, 1, "FL: ");
                OLED_ShowNum(40, 1, g_value_4);
                OLED_ShowString(0, 1, "->");  // Initial cursor position on FR
            } 
            break;
            
        case 2: // Debug LED (was case 3 before)
            first_entry = 1;  // Reset status 
            Update_Led(0, 0);
            break;
				case 3:
						OLED_Clear();
						break ;
    }
    
    Reset_Status();
    
    while(1) {
        Check_Status();
        uint32_t current_time = millis();
        
        // Update all sensor values periodically for all menus
        if(current_time - last_update_time >= UPDATE_INTERVAL) {
            last_update_time = current_time;
            
            // Update all global values at once
            get_value();
            
            // Handle MPU calibration menu updates
            if(menu == 0) {
                OLED_ClearArea(75, 0, 5);
                OLED_ShowFloat(75, 0, g_angle);
                OLED_ClearArea(75, 1, 5);
                OLED_ShowFloat(75, 1, calibrated_angle);
            } 
        }
        
        // Handle MPU calibration menu
        if(menu == 0) {
            // Handle MPU calibration menu with only 3 options now
            Handle_Calibration_Navigation(&calib_cursor_pos, 3);

            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Clear old cursor positions
                OLED_ShowString(0, 0, "   ");
                OLED_ShowString(0, 1, "   ");
                OLED_ShowString(0, 2, "   ");
                
                // Draw new cursor
                OLED_ShowString(0, calib_cursor_pos, "->");
                
                last_calib_cursor_pos = calib_cursor_pos;
            }

            // Process selection
            if(flag_stt.flag_3) {
                if(calib_cursor_pos == 0) {
                    // "Angle" selection - save angle
                    // Get fresh value before saving
                    get_value();
                    calibrated_angle = g_angle;
                    
                    OLED_Clear();
                    OLED_ShowString(10, 2, "Angle Saved!");
                    delay_ms(1000);
                    
                    // Redraw menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, g_angle);
                    OLED_ShowString(25, 1, "Saved:");
                    OLED_ShowFloat(75, 1, calibrated_angle);
                    OLED_ShowString(25, 2, "Exit");
                    OLED_ShowString(0, calib_cursor_pos, "->");
                    
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 1) {
                    // "Saved" selection - clear the saved angle
                    calibrated_angle = 0;
                    
                    OLED_Clear();
                    OLED_ShowString(10, 2, "Angle Cleared!");
                    delay_ms(1000);
                    
                    // Redraw menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, g_angle);
                    OLED_ShowString(25, 1, "Saved:");
                    OLED_ShowFloat(75, 1, calibrated_angle);
                    OLED_ShowString(25, 2, "Exit");
                    OLED_ShowString(0, calib_cursor_pos, "->");
                    
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 2) {
                    // "Exit" option
                    break;
                }
            }
        }
        
        // Handle LED calibration menu
        else if(menu == 1) {
            // Initialize: Show LED values on first entry
            OLED_Clear();
            OLED_ShowString(25, 0, "FR:");
            OLED_ShowNum(70, 0, g_value_1);
            OLED_ShowString(25, 2, "R:");
            OLED_ShowNum(70, 2, g_value_2);
            OLED_ShowString(25, 3, "L:");
            OLED_ShowNum(70, 3, g_value_3);
            OLED_ShowString(25, 1, "FL:");
            OLED_ShowNum(70, 1, g_value_4);
            OLED_ShowString(0, 0, "->");  // Initial cursor position
            
            // Handle LED calibration menu
            calib_cursor_pos = 0;
            last_calib_cursor_pos = 255; // Force initial draw
            
            while(1) {
                Check_Status();
                uint32_t current_time = millis();
                
                // Update values periodically
                if(current_time - last_update_time >= UPDATE_INTERVAL) {
                    last_update_time = current_time;
                    get_value();
                    
                    // Only update visible LED values if on first page
                    if(calib_cursor_pos < 4) {
                        OLED_ClearArea(70, 0, 5);
                        OLED_ShowNum(70, 0, g_value_1);
                        OLED_ClearArea(70, 2, 5);
                        OLED_ShowNum(70, 2, g_value_2);
                        OLED_ClearArea(70, 3, 5);
                        OLED_ShowNum(70, 3, g_value_3);
                        OLED_ClearArea(70, 1, 5);
                        OLED_ShowNum(70, 1, g_value_4);
                        
                        // Show asterisk for calibrated LEDs
                    }
                }
                
                // Navigation
                if(flag_stt.flag_1) {
                    flag_stt.flag_1 = 0;
                    calib_cursor_pos++;
                    if(calib_cursor_pos >= 7) {
                        calib_cursor_pos = 0;
                    }
                    
                    // Handle page transition
                    if((calib_cursor_pos == 4 && last_calib_cursor_pos < 4) || 
                       (calib_cursor_pos < 4 && last_calib_cursor_pos >= 4)) {
                        last_calib_cursor_pos = 255; // Force redraw
                    }
                }
                
                if(flag_stt.flag_2) {
                    flag_stt.flag_2 = 0;
                    if(calib_cursor_pos == 0) {
                        calib_cursor_pos = 6;
                    } else {
                        calib_cursor_pos--;
                    }
                    
                    // Handle page transition
                    if((calib_cursor_pos == 3 && last_calib_cursor_pos >= 4) || 
                       (calib_cursor_pos >= 4 && last_calib_cursor_pos < 4)) {
                        last_calib_cursor_pos = 255; // Force redraw
                    }
                }
                
                // Redraw if cursor position changed
                if(calib_cursor_pos != last_calib_cursor_pos) {
                    OLED_Clear();
                    
                    // Draw the appropriate page
                    if(calib_cursor_pos < 4) {
                        // LED values page
                        OLED_ShowString(25, 0, "FR:");
                        OLED_ShowNum(70, 0, g_value_1);
                        OLED_ShowString(25, 2, "R:");
                        OLED_ShowNum(70, 2, g_value_2);
                        OLED_ShowString(25, 3, "L:");
                        OLED_ShowNum(70, 3, g_value_3);
                        OLED_ShowString(25, 1, "FL:");
                        OLED_ShowNum(70, 1, g_value_4);
                        
                        // Draw cursor at current position
                        OLED_ShowString(0, calib_cursor_pos, "->");
                    } else {
                        // Menu options page
                        OLED_ShowString(25, 0, "Show Values");
                        OLED_ShowString(25, 1, "Clear All");
                        OLED_ShowString(25, 2, "Exit");
                        
                        // Draw cursor at current position (adjusted for page)
                        OLED_ShowString(0, calib_cursor_pos - 4, "->");
                    }
                    
                    last_calib_cursor_pos = calib_cursor_pos;
                }
                
                // Handle selection
                if(flag_stt.flag_3) {
                    flag_stt.flag_3 = 0;
                    
                    if(calib_cursor_pos < 4) {
                        // LED selection - save or clear this LED
                        uint8_t led_idx = calib_cursor_pos;
                        
                        if(calibrated_leds[led_idx] != 0) {
                            // Already calibrated, clear it
                            calibrated_leds[led_idx] = 0;
                            
                            OLED_Clear();
                            OLED_ShowString(10, 2, "LED");
                            OLED_ShowNum(40, 2, led_idx + 1);
                            OLED_ShowString(50, 2, " Cleared");
                            delay_ms(1000);
                        } else {
                            // Save current value
                            get_value();
                            uint16_t value = 0;
                            switch(led_idx) {
                                case 0: value = g_value_1; break; //FR_S
                                case 1: value = g_value_4; break; //FL_s
                                case 2: value = g_value_2; break; // R_S
                                case 3: value = g_value_3; break; // L_S
                            }
                            calibrated_leds[led_idx] = value;
                            
                            OLED_Clear();
                            OLED_ShowString(10, 2, "LED");
                            OLED_ShowNum(40, 2, led_idx + 1);
                            OLED_ShowString(50, 2, " Saved");
                            delay_ms(1000);
                        }
                        
                        // Redraw current page
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 4) {
                        // "Show Values" option
                        OLED_Clear();
                        OLED_ShowString(0, 0, "FR:");
                        OLED_ShowNum(40, 0, calibrated_leds[0]);
                        OLED_ShowString(0, 2, "R:");
                        OLED_ShowNum(40, 2, calibrated_leds[2]);
                        OLED_ShowString(0, 3, "L:");
                        OLED_ShowNum(40, 3, calibrated_leds[3]);
                        OLED_ShowString(0, 1, "FL:");
                        OLED_ShowNum(40, 1, calibrated_leds[1]);

                        
                        // Wait for long press to exit
                        uint32_t show_press_start = 0;
                        uint8_t show_is_button_held = 0;
                        
                        while(1) {
                            Check_Status();
                            uint32_t curr_time = millis();
                            // Check for button press and hold
                            if(stt_3 == 0) { // Button is pressed
                                if(!show_is_button_held) {
                                    // Button just pressed, record start time
                                    show_press_start = curr_time;
                                    show_is_button_held = 1;
                                } else {
                                    // Check if button has been held long enough
                                    if(curr_time - show_press_start >= LONG_PRESS_DURATION) {
                                        // Long press detected, exit loop
                                        break;
                                    }
                                }
                            } else {
                                // Button released
                                show_is_button_held = 0;
                            }
                            
                            // Essential delay to prevent CPU overload
                            delay_ms(10);
                        }
                        
                        // Force redraw when returning
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 5) {
                        // "Clear All" option
                        for(uint8_t i = 0; i < 4; i++) {
                            calibrated_leds[i] = 0;
                        }
                        
                        OLED_Clear();
                        OLED_ShowString(10, 2, "Values Cleared");
                        delay_ms(1000);
                        
                        // Force redraw when returning
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 6) {
                        // Exit - break out of the menu loop
                        break;
                    }
                }
                
                // Check for long press to exit (only on LED pages)
                if(calib_cursor_pos < 4) {
                    if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                        break;
                    }
                }
            }
            break;
        }
        
        // Handle Debug LED menu 
        else if(menu == 2) {
            static uint8_t debug_page = 0; // 0 = current values, 1 = saved values
            
            // Initialize on first entry
            OLED_Clear();
            // Force redraw
            first_entry = 1;
            Update_Led(debug_page, 0);
            
            while(1) {
                Check_Status();
                uint32_t current_time = millis();
                
                // Update values periodically (only for current values page)
                if(current_time - last_update_time >= UPDATE_INTERVAL && debug_page == 0) {
                    last_update_time = current_time;
                    get_value();
                    Update_Led(debug_page, 0);
                }
                
                // Toggle between pages
                if(flag_stt.flag_1 || flag_stt.flag_2) {
                    debug_page = !debug_page;
                    
                    // Force full redraw of values with new page
                    first_entry = 1;
                    Update_Led(debug_page, 0);
                    
                    flag_stt.flag_1 = 0;
                    flag_stt.flag_2 = 0;
                }
                
                // Check for long press to exit
                if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                    break;
                }
                
                // Add small delay to prevent CPU overload
                delay_ms(10);
            }
            break;
        }
    }
    
    // Handle transition back to main menu
    OLED_Clear();
    Reset_Status();
}

// Modified Update_Led function for Debug LED
void Update_Led(uint8_t show_calibrated, uint8_t led_cursor_pos)
{
    // Reset static variable if this is entry to Debug LED menu (led_cursor_pos == 0)
    static uint8_t current_mode = 255;

    // Luôn vẽ lại nhãn khi lần đầu vào menu từ Select_Menu
    // Hoặc khi chuyển chế độ hiển thị
    if(first_entry || current_mode != show_calibrated) {
        OLED_Clear();
        // Hiển thị các nhãn
        OLED_ShowString(15, 0, "FR:");
        OLED_ShowString(15, 2, "R:");
        OLED_ShowString(15, 3, "L:");
        OLED_ShowString(15, 1, "FL:");
        
        
        first_entry = 0;
        current_mode = show_calibrated;
    }
    
    // Cập nhật giá trị (chỉ số, không xóa cả dòng)
    if(!show_calibrated) {
        // Hiển thị giá trị hiện tại
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, g_value_1);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, g_value_2);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, g_value_3);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, g_value_4);
    } else {
        // Hiển thị giá trị đã lưu
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, calibrated_leds[0]);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, calibrated_leds[2]);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, calibrated_leds[3]);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, calibrated_leds[1]);
    }
}

void get_value(void)
{
    g_angle = MPU6050.Get_Yaw(0); // Replace with actual sensor reading
    g_value_1 = IRSensor_data[FR_S]; // Replace with actual sensor reading
    g_value_2 = IRSensor_data[R_S]; // Replace with actual sensor reading
    g_value_3 = IRSensor_data[L_S]; // Replace with actual sensor reading
    g_value_4 = IRSensor_data[FL_S]; // Replace with actual sensor reading
}