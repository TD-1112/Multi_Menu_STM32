#include "menu.h"

flag_status flag_stt;

uint16_t stt_1;
uint16_t stt_2;
uint16_t stt_3;

float calibrated_angle = 0;
uint16_t calibrated_leds[4];

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

void Main_Menu(uint16_t value_1, uint16_t value_2, uint16_t value_3, 
               uint16_t value_4, uint16_t value_5, uint16_t value_6,
               float angle)   
{
    Check_Status();
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
        // Call Select_Menu with the current cursor position
        Select_Menu(cursor_pos, value_1, value_2, value_3, value_4, value_5, value_6, angle);
        // Force redraw of menu when we return
        menu_initialized = 0; 
        last_drawn_pos = 255;
        
        flag_stt.flag_3 = 0;
    }
}

// New animation dots function
void Update_Animation_Dots(uint32_t current_time, uint32_t *last_animation_time, uint8_t *dot_state)
{
    if(current_time - *last_animation_time >= ANIMATION_INTERVAL) {
        *last_animation_time = current_time;
        
        // Clear the dots area first
        OLED_ShowString(100, 2, "    ");
        
        // Display dots based on current state
        if(*dot_state == 1) {
            OLED_ShowString(90, 2, ".");
        } else if(*dot_state == 2) {
            OLED_ShowString(90, 2, "..");
        } else if(*dot_state == 3) {
            OLED_ShowString(90, 2, "...");
        }
        
        // Move to next state
        *dot_state = (*dot_state + 1) % 4; // Cycle through 0,1,2,3
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
void Handle_LED_Navigation(uint16_t value_1, uint16_t value_2, uint16_t value_3, 
                          uint16_t value_4, uint16_t value_5, uint16_t value_6, 
                          uint8_t *led_cursor_pos, uint8_t *led_page)
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
        Update_Led(value_1, value_2, value_3, value_4, value_5, value_6, *led_cursor_pos, 1);
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
        Update_Led(value_1, value_2, value_3, value_4, value_5, value_6, *led_cursor_pos, 1);
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
    
    // Handle Up button (flag_stt.flag_2)
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
void Select_Menu(uint8_t menu, uint16_t value_1, uint16_t value_2, uint16_t value_3, 
                uint16_t value_4, uint16_t value_5, uint16_t value_6, float angle)
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
            OLED_ShowString(0, 0, "Calib MPU");
            OLED_ShowString(30, 1, "Angle: ");
            OLED_ShowNum(90, 1, angle);
            OLED_ShowString(30, 2, "Save");
            OLED_ShowString(30, 3, "Exit");
            OLED_ShowString(0, 2, "->");  // Uncomment this line to draw initial cursor
            break;
            
        case 1: // Calib LED
            OLED_ShowString(20, 0, "Calib LED");
            // Initial screen shows LED values
            OLED_ShowString(0, 1, "LED1: ");
            OLED_ShowNum(40, 1, value_1);
            OLED_ShowString(0, 2, "LED2: ");
            OLED_ShowNum(40, 2, value_2);
            OLED_ShowString(0, 3, "LED3: ");
            OLED_ShowNum(40, 3, value_3);
            OLED_ShowString(90, 1, "Next ->");
            break;
            
        case 2: // Debug MPU
            Update_MPU(angle);
            break;
            
        case 3: // Debug LED
            Update_Led(value_1, value_2, value_3, value_4, value_5, value_6, 0, 1);
            break;
    }
    
    Reset_Status();
    
    // Wait in this menu until button 3 is held for 1 second or selection is made
    while(1) {
       Check_Status();
        uint32_t current_time = millis();
        
        // Handle MPU calibration menu
        if(menu == 0) {

            
            // Check the position against the last saved position
            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Clear BOTH possible cursor positions with 3 spaces
                OLED_ShowString(0, 2, "   ");  // Use 3 spaces instead of 2
                OLED_ShowString(0, 3, "   ");  // Use 3 spaces instead of 2
                
                // Draw complete cursor as a string (not character by character)
                OLED_ShowString(0, 2 + calib_cursor_pos, "->");
                
                // Update last known position
                
                last_calib_cursor_pos = calib_cursor_pos;
            }
            // Handle navigation first
            Handle_Calibration_Navigation(&calib_cursor_pos, 2);

            // Handle selection
            if(flag_stt.flag_3) {
                if(calib_cursor_pos == 0) {
                    // "Save" selected - Save calibration
                    calibrated_angle = angle;
                    
                    // Show confirmation
                    OLED_Clear();
                    OLED_ShowString(15, 2, "MPU Calibrated!");
                    delay_ms(1000);
                }
                // Exit for both Yes and No
                break;
            }
            
            // Update angle value periodically
            if(current_time - last_update_time >= UPDATE_INTERVAL) {
                last_update_time = current_time;
                
                // Update the angle display
                OLED_ClearArea(90, 1, 5);
                OLED_ShowNum(90, 1, angle);
            }
        }
        
        // Handle LED calibration menu
        if(menu == 1) {
            // Handle different pages in LED calibration
            if(calib_page == 0) {
                // First page - show LED values and Next button
                
                // Update LED values periodically
                if(current_time - last_update_time >= UPDATE_INTERVAL) {
                    last_update_time = current_time;
                    
                    OLED_ClearArea(40, 1, 5);
                    OLED_ShowNum(40, 1, value_1);
                    
                    OLED_ClearArea(40, 2, 5);
                    OLED_ShowNum(40, 2, value_2);
                    
                    OLED_ClearArea(40, 3, 5);
                    OLED_ShowNum(40, 3, value_3);
                }
                
                // Check if Next is selected
                if(flag_stt.flag_3) {
                    // Change to Yes/No page
                    calib_page = 1;
                    OLED_Clear();
                    
                    // Draw second page
                    OLED_ShowString(20, 0, "Calib LED");
                    OLED_ShowString(0, 1, "LED4: ");
                    OLED_ShowNum(40, 1, value_4);
                    OLED_ShowString(15, 2, "Save");
                    OLED_ShowString(15, 3, "Exit");
                    OLED_ShowString(0, 2, "->");  // Initial cursor position
                    
                    // Reset cursor position for Yes/No selection
                    calib_cursor_pos = 0;
                    last_calib_cursor_pos = 255;
                    flag_stt.flag_3 = 0;
                }
            }
            else {  // calib_page == 1
                // Second page - LED4 value and Yes/No options
                
                // Navigate between Yes/No options
                if(calib_cursor_pos != last_calib_cursor_pos) {
                    // Clear previous cursor position
                    OLED_ShowString(0, 2, "   ");  // Using 3 spaces instead of 2
                    OLED_ShowString(0, 3, "   ");  // Using 3 spaces instead of 2
                    
                    // Show cursor at new position
                    OLED_ShowString(0, 2 + calib_cursor_pos, "->");
                    
                    last_calib_cursor_pos = calib_cursor_pos;
                }
                
                // Handle navigation
                Handle_Calibration_Navigation(&calib_cursor_pos, 2);
                
                // Update LED4 value periodically
                if(current_time - last_update_time >= UPDATE_INTERVAL) {
                    last_update_time = current_time;
                    
                    OLED_ClearArea(40, 1, 5);
                    OLED_ShowNum(40, 1, value_4);
                }
                
                // Handle selection
                if(flag_stt.flag_3) {
                    if(calib_cursor_pos == 0) {
                        // "Save" selected - Save LED calibration values

                        calibrated_leds[0] = value_1;
                        calibrated_leds[1] = value_2;
                        calibrated_leds[2] = value_3;
                        calibrated_leds[3] = value_4;
                        
                        // Show confirmation
                        OLED_Clear();
                        OLED_ShowString(15, 2, "LEDs Calibrated!");
                        delay_ms(1000);
                    }
                    // Exit for both Save and Cancel
                    break;
                }
            }
        }
        
        // LED menu navigation handling for Debug LED
        if(menu == 3) {
            Handle_LED_Navigation(value_1, value_2, value_3, value_4, value_5, value_6, 
                                &led_cursor_pos, &led_page);
        }
        
        // Long press detection for button 3 (only for debug menus)
        if((menu == 2 || menu == 3) && 
           Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
            break; // Exit the menu
        }
        
        // Update data for Debug menus
        if((menu == 2 || menu == 3) && (current_time - last_update_time >= UPDATE_INTERVAL)) {
            last_update_time = current_time;
            
            if(menu == 2) { // Debug MPU
                OLED_ClearArea(50, 1, 6);
                OLED_ShowNum(50, 1, angle);
            } 
            else if(menu == 3) { // Debug LED
                Update_Led(value_1, value_2, value_3, value_4, value_5, value_6, led_cursor_pos, 0);
            }
        }
    }
    
    // Handle transition back to main menu
    OLED_Clear();
    Reset_Status();
}

// Modified Update_Led function
void Update_Led(uint16_t value_1, uint16_t value_2, uint16_t value_3, uint16_t value_4, 
                uint16_t value_5, uint16_t value_6, uint8_t cursor_pos, uint8_t redraw_labels)
{
    static uint8_t last_cursor_pos = 255;  // Track last cursor position
    static uint8_t last_page = 255;        // Track last page
    
    uint8_t current_page = cursor_pos / 4; // 4 LEDs per page
    uint8_t display_row = cursor_pos % 4;
    
    // Only redraw labels if requested or page changed
    if(redraw_labels || current_page != last_page) {
        if(current_page == 0) {
            // Page 0: LEDs 1-4
            OLED_ShowString(15, 0, "LED1:");
            OLED_ShowString(15, 1, "LED2:");
            OLED_ShowString(15, 2, "LED3:");
            OLED_ShowString(15, 3, "LED4:");
        } else {
            // Page 1: LEDs 5-6
            OLED_ShowString(15, 0, "LED5:");
            OLED_ShowString(15, 1, "LED6:");
            // Clear unused rows from previous page if needed
            if(last_page == 0) {
                OLED_ClearLine(2);
                OLED_ClearLine(3);
            }
        }
        last_page = current_page;
    }
    
    // Only update cursor if position changed
    if(cursor_pos != last_cursor_pos) {
        uint8_t rows_to_clear = (current_page == 0) ? 4 : 2;
        
        // Clear old cursor position if valid
        if(last_cursor_pos != 255) {
            uint8_t old_row = last_cursor_pos % 4;
            // Only clear if on same page or full redraw
            if((last_cursor_pos / 4 == current_page) || redraw_labels) {
                OLED_ShowString(0, old_row, "  ");
            }
        }
        
        // Draw cursor at new position
        OLED_ShowString(0, display_row, "->");
        last_cursor_pos = cursor_pos;
    }
    
    // Update only the value areas
    if(current_page == 0) {
        // Page 0: Update LEDs 1-4
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, value_1);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, value_2);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, value_3);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, value_4);
    } else {
        // Page 1: Update LEDs 5-6
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, value_5);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, value_6);
    }
}

// Modified Update_MPU function
void Update_MPU(float value)
{
    static uint8_t first_draw = 1;
    
    if(first_draw) {
        // First time draw - draw all elements
        OLED_ShowString(30, 0, "MPU Debug");
        OLED_ShowString(0, 1, "Gyro: ");
        first_draw = 0;
    }
    
    // Just update the value
    OLED_ClearArea(50, 1, 6);  // Clear value area
    OLED_ShowNum(50, 1, value);
}