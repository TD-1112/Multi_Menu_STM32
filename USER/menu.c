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
										flag = 0;
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
        OLED_ShowString(20, 0, "Calib MPU");
        OLED_ShowString(20, 1, "Calib Led");
        OLED_ShowString(20, 2, "Debug MPU");
        OLED_ShowString(20, 3, "Debug Led");
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
    
    // Handle Up button (flag_stt.flag_2) {  // THÊM ĐIỀU KIỆN KIỂM TRA Ở ĐÂY
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
            OLED_ShowString(25, 1, "Show Value");
            OLED_ShowString(25, 2, "Clear Value");
            OLED_ShowString(25, 3, "Exit");
            OLED_ShowString(0, 0, "->");  // Initial cursor position on Angle
            break;
            
        case 1: // Calib LED
            if(calib_page == 0) {
                // Trang 1: Hiển thị giá trị LED hiện tại
                OLED_ShowString(0, 0, "LED1: ");
                OLED_ShowNum(40, 0, g_value_1);
                OLED_ShowString(0, 1, "LED2: ");
                OLED_ShowNum(40, 1, g_value_2);
                OLED_ShowString(0, 2, "LED3: ");
                OLED_ShowNum(40, 2, g_value_3);
                OLED_ShowString(0, 3, "LED4: ");
                OLED_ShowNum(40, 3, g_value_4);
                OLED_ShowString(0, 1, "->");  // Initial cursor position on LED1
            } 
            break;
            
        case 2: // Debug MPU
            Update_MPU();
            break;
            
        case 3: // Debug LED
            first_entry = 1;  // Reset trạng thái để vẽ lại toàn bộ màn hình
            Update_Led(0, 0);
            break;
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
            } 
            // Handle LED calibration menu updates
            else if(menu == 1) {
                uint8_t start_pos = 0;
                if(calib_cursor_pos > 2) {
                    start_pos = calib_cursor_pos - 2;
                }
                if(start_pos > 3) {
                    start_pos = 3;
                }
                
                // Update only visible LEDs on screen
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 4; i++) {
                    uint8_t led_idx = start_pos + i;
                    uint16_t value;
                    
                    switch(led_idx) {
                        case 0: value = g_value_1; break;
                        case 1: value = g_value_2; break;
                        case 2: value = g_value_3; break;
                        case 3: value = g_value_4; break;
                        default: value = 0;
                    }
                    
                    OLED_ClearArea(70, i, 5);
                    OLED_ShowNum(70, i, value);
                }
            }
            // Handle Debug MPU menu updates
            else if(menu == 2) {
                Update_MPU();
            }
            // Handle Debug LED menu updates (only for current values page)
            else if(menu == 3) {
                static uint8_t last_show_calibrated = 255;
                static uint8_t show_calibrated = 0;
                
                // Only update current values mode, not calibrated values mode
                if(!show_calibrated) {
                    // Update LED values on screen
                    OLED_ClearArea(60, 0, 5);
                    OLED_ShowNum(60, 0, g_value_1);
                    
                    OLED_ClearArea(60, 1, 5);
                    OLED_ShowNum(60, 1, g_value_2);
                    
                    OLED_ClearArea(60, 2, 5);
                    OLED_ShowNum(60, 2, g_value_3);
                    
                    OLED_ClearArea(60, 3, 5);
                    OLED_ShowNum(60, 3, g_value_4);
                }
                
                // Store the current mode to detect changes
                last_show_calibrated = show_calibrated;
            }
        }
        
        // Handle MPU calibration menu
        if(menu == 0) {
            // Navigation and cursor handling for 4 options
            Handle_Calibration_Navigation(&calib_cursor_pos, 4);

            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Clear old cursor positions
                OLED_ShowString(0, 0, "   ");
                OLED_ShowString(0, 1, "   ");
                OLED_ShowString(0, 2, "   ");
                OLED_ShowString(0, 3, "   ");
                
                // Draw new cursor
                OLED_ShowString(0, calib_cursor_pos, "->");
                
                last_calib_cursor_pos = calib_cursor_pos;
            }

            // Process selection
            if(flag_stt.flag_3) {
                if(calib_cursor_pos == 0) {
                    // "Angle" selection - save or clear angle
                    if(calibrated_angle != 0) {
                        calibrated_angle = 0;
                        
                        OLED_Clear();
                        OLED_ShowString(10, 2, "Angle Cleared!");
                        delay_ms(1000);
                    } else {
                        // Get fresh value before saving
                        get_value();
                        calibrated_angle = g_angle;
                        
                        OLED_Clear();
                        OLED_ShowString(10, 2, "Angle Saved!");
                        delay_ms(1000);
                    }
                    
                    // Redraw menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, g_angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    OLED_ShowString(0, 0, "->");
                    
                    calib_cursor_pos = 0;
                    last_calib_cursor_pos = 0;
                    flag_stt.flag_3 = 0;
                }
                // Rest of the code remains unchanged
                else if(calib_cursor_pos == 1) {
                    // "Show Value" option
                    OLED_Clear();
                    OLED_ShowString(0, 0, "Angle:");
                    OLED_ShowFloat(55, 0, calibrated_angle);
                    
                    uint32_t show_press_start = 0;
                    uint8_t show_is_button_held = 0;
                    
                    while(1) {
                        Check_Status();
                        uint32_t current_time = millis();
                        
                        if(Check_Long_Press(stt_3, current_time, &show_press_start, &show_is_button_held)) {
                            break;
                        }
                    }
                    
                    // Redraw menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    get_value(); // Get fresh value before redrawing
                    OLED_ShowFloat(75, 0, g_angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    OLED_ShowString(0, 1, "->");
                    
                    calib_cursor_pos = 1;
                    last_calib_cursor_pos = 1;
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 2) {
                    // "Clear Value" option
                    calibrated_angle = 0;
                    
                    OLED_Clear();
                    OLED_ShowString(10, 2, "Angle Cleared!");
                    delay_ms(1000);
                    
                    // Redraw menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    get_value(); // Get fresh value before redrawing
                    OLED_ShowFloat(75, 0, g_angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    OLED_ShowString(0, 2, "->");
                    
                    calib_cursor_pos = 2;
                    last_calib_cursor_pos = 2;
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 3) {
                    // "Exit" option
                    break;
                }
            }
        }
        
        // Handle LED calibration menu
        else if(menu == 1) {
            // Navigation between 7 items: 4 LEDs, Show Value, Clear Value, Exit
            Handle_Calibration_Navigation(&calib_cursor_pos, 7);

            // Check if cursor position changed - redraw menu if necessary
            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Calculate which items to display based on cursor position
                // Show 4 items at a time with current selection centered when possible
                uint8_t start_pos = 0;
                if(calib_cursor_pos > 2) {
                    start_pos = calib_cursor_pos - 2;
                }
                if(start_pos > 3) {
                    start_pos = 3;
                }
                
                // Clear screen before drawing new items
                OLED_Clear();
                
                // Draw the visible menu items (maximum 4 items)
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 7; i++) {
                    uint8_t item = start_pos + i;
                    
                    if(item < 4) {
                        // LED1-4 items
                        OLED_ShowString(25, i, "LED");
                        OLED_ShowNum(50, i, item+1);
                        OLED_ShowString(60, i, ":");
                        
                        // Show current LED value
                        uint16_t value = 0;
                        switch(item) {
                            case 0: value = g_value_1; break;
                            case 1: value = g_value_2; break;
                            case 2: value = g_value_3; break;
                            case 3: value = g_value_4; break;
                        }
                        OLED_ShowNum(70, i, value);
                        
                        // Show asterisk next to LEDs with saved calibration values
                        if(calibrated_leds[item] != 0) {
                            OLED_ShowString(100, i, "*");
                        }
                    }
                    else if(item == 4) {
                        OLED_ShowString(25, i, "Show Values");
                    }
                    else if(item == 5) {
                        OLED_ShowString(25, i, "Clear All");
                    }
                    else if(item == 6) {
                        OLED_ShowString(25, i, "Exit");
                    }
                }
                
                // Calculate where to draw the cursor on screen
                uint8_t cursor_y = calib_cursor_pos - start_pos;
                
                // Draw cursor at appropriate position
                OLED_ShowString(0, cursor_y, "->");
                
                last_calib_cursor_pos = calib_cursor_pos;
            }
            
            // Update LED values periodically if we're showing LEDs (only update visible LEDs)
            if(current_time - last_update_time >= UPDATE_INTERVAL) {
                last_update_time = current_time;
                
                // Calculate which items are currently visible
                uint8_t start_pos = 0;
                if(calib_cursor_pos > 2) {
                    start_pos = calib_cursor_pos - 2;
                }
                if(start_pos > 3) {
                    start_pos = 3;
                }
                
                // Update only visible LED values (not labels or menu items)
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 4; i++) {
                    uint8_t led_idx = start_pos + i;
                    uint16_t value = 0;
                    
                    switch(led_idx) {
                        case 0: value = g_value_1; break;
                        case 1: value = g_value_2; break;
                        case 2: value = g_value_3; break;
                        case 3: value = g_value_4; break;
                    }
                    
                    // Clear area and show updated value
                    OLED_ClearArea(70, i, 5);
                    OLED_ShowNum(70, i, value);
                }
            }
            
            // Handle selection (Button 3)
            if(flag_stt.flag_3) {
                flag_stt.flag_3 = 0;  // Reset flag immediately to prevent multiple triggers
                
                if(calib_cursor_pos < 4) {
                    // LED selection - toggle save/clear for this LED
                    uint8_t current_led = calib_cursor_pos;
                    
                    if(calibrated_leds[current_led] != 0) {
                        // Already calibrated, clear the value
                        calibrated_leds[current_led] = 0;
                        
                        // Show confirmation
                        OLED_Clear();
                        OLED_ShowString(10, 1, "LED");
                        OLED_ShowNum(35, 1, current_led+1);
                        OLED_ShowString(40, 1, " Cleared!");
                        delay_ms(1000);
                    } else {
                        // Not calibrated, save current value
                        get_value();  // Get fresh reading first
                        
                        uint16_t value_to_save = 0;
                        switch(current_led) {
                            case 0: value_to_save = g_value_1; break;
                            case 1: value_to_save = g_value_2; break;
                            case 2: value_to_save = g_value_3; break;
                            case 3: value_to_save = g_value_4; break;
                        }
                        
                        calibrated_leds[current_led] = value_to_save;
                        
                        // Show confirmation
                        OLED_Clear();
                        OLED_ShowString(10, 1, "LED");
                        OLED_ShowNum(35, 1, current_led+1);
                        OLED_ShowString(40, 1, " Saved!");
                        delay_ms(1000);
                    }
                    
                    // Force redraw menu after returning
                    last_calib_cursor_pos = 255;
                }
                else if(calib_cursor_pos == 4) {
                    // "Show Values" option - display all saved values
                    OLED_Clear();
                    //OLED_ShowString(0, 0, "Saved Values:");
                    
                    for(uint8_t i = 0; i < 4; i++) {
                        OLED_ShowString(0, i, "LED");
                        OLED_ShowNum(25, i, i);
                        OLED_ShowString(35, i, ":");
                        OLED_ShowNum(50, i+1, calibrated_leds[i]);
                    }
                    
                    // Wait for long button press to return
                    uint32_t show_press_start = 0;
                    uint8_t show_is_button_held = 0;
                    
                    while(1) {
                        Check_Status();
                        uint32_t current_time = millis();
                        
                        if(Check_Long_Press(stt_3, current_time, &show_press_start, &show_is_button_held)) {
                            break;
                        }
                    }
                    
                    // Force redraw menu after returning
                    last_calib_cursor_pos = 255;
                }
                else if(calib_cursor_pos == 5) {
                    // "Clear All" option - clear all calibrated values
                    for(uint8_t i = 0; i < 4; i++) {
                        calibrated_leds[i] = 0;
                    }
                    
                    // Show confirmation
                    OLED_Clear();
                    OLED_ShowString(10, 2, "All Values Cleared!");
                    delay_ms(1000);
                    
                    // Force redraw menu after returning
                    last_calib_cursor_pos = 255;
                }
                else if(calib_cursor_pos == 6) {
                    // "Exit" option - return to main menu
                    break;
                }
            }
        }
        
        // Handle Debug LED menu 
        else if(menu == 3) {
            static uint8_t show_calibrated = 0;
            
            // Toggle between current and calibrated values
            if(flag_stt.flag_1 || flag_stt.flag_2) {
                show_calibrated = !show_calibrated;
                get_value(); // Get fresh values before updating display
                Update_Led(show_calibrated, 0);
                flag_stt.flag_1 = 0;
                flag_stt.flag_2 = 0;
            }
            
            // Long press to exit
            if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                break;
            }
        }
        
        // Handle long press detection for debug menus
        else if((menu == 2 || menu == 3) && 
           Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
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
        OLED_ShowString(15, 0, "LED1:");
        OLED_ShowString(15, 1, "LED2:");
        OLED_ShowString(15, 2, "LED3:");
        OLED_ShowString(15, 3, "LED4:");
        
        first_entry = 0;
        current_mode = show_calibrated;
    }
    
    // Cập nhật giá trị (chỉ số, không xóa cả dòng)
    if(!show_calibrated) {
        // Hiển thị giá trị hiện tại
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, g_value_1);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, g_value_2);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, g_value_3);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, g_value_4);
    } else {
        // Hiển thị giá trị đã lưu
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, calibrated_leds[0]);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, calibrated_leds[1]);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, calibrated_leds[2]);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, calibrated_leds[3]);
    }
}

// Modified Update_MPU function
void Update_MPU(void)
{
    static uint8_t first_draw = 1;
    
    // Update values before displaying
    get_value();
    
    // Luôn vẽ lại toàn bộ nội dung khi vào menu Debug MPU
    OLED_ShowString(25, 0, "MPU Debug");
    OLED_ShowString(0, 1, "Calibrated:");
    OLED_ShowString(0, 2, "Current:");
    
    // Cập nhật cả hai giá trị
    OLED_ClearArea(90, 1, 6);  // Xóa vùng hiển thị giá trị đã hiệu chuẩn
    OLED_ShowFloat(90, 1, calibrated_angle);
    
    OLED_ClearArea(70, 2, 6);  // Xóa vùng hiển thị giá trị hiện tại
    OLED_ShowFloat(70, 2, g_angle);
}


void get_value(void)
{
    g_angle = MPU6050.Get_Yaw(0);
    g_value_1 = 10;  // Replace with actual sensor reading
    g_value_2 = 10;  // Replace with actual sensor reading
    g_value_3 = 10;  // Replace with actual sensor reading
    g_value_4 = 10;  // Replace with actual sensor reading
}