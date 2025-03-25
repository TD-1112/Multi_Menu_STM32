#include "menu.h"

flag_status flag_stt;

uint16_t stt_1;
uint16_t stt_2;
uint16_t stt_3;

float calibrated_angle = 0;
uint16_t calibrated_leds[4];
static uint8_t first_entry = 1;  // Thêm biến để kiểm tra lần đầu vào menu

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
               uint16_t value_4, float angle)   
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
        // Call Select_Menu with the current cursor position
        Select_Menu(cursor_pos, value_1, value_2, value_3, value_4, angle);
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
                          uint16_t value_4, uint8_t *led_cursor_pos, uint8_t *led_page)
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
        Update_Led(value_1, value_2, value_3, value_4, 0, *led_cursor_pos);
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
        Update_Led(value_1, value_2, value_3, value_4, 0, *led_cursor_pos);
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
void Select_Menu(uint8_t menu, uint16_t value_1, uint16_t value_2, uint16_t value_3, 
                uint16_t value_4,  float angle)
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
            // Hiển thị menu ban đầu
            OLED_ShowString(25, 0, "Angle: ");
            OLED_ShowFloat(75, 0, angle);
            OLED_ShowString(25, 1, "Show Value");
            OLED_ShowString(25, 2, "Clear Value");
            OLED_ShowString(25, 3, "Exit");
            OLED_ShowString(0, 0, "->");  // Initial cursor position on Angle
            break;
            
        case 1: // Calib LED
            if(calib_page == 0) {
                // Trang 1: Hiển thị giá trị LED hiện tại
                OLED_ShowString(0, 1, "LED1: ");
                OLED_ShowNum(40, 1, value_1);
                OLED_ShowString(0, 2, "LED2: ");
                OLED_ShowNum(40, 2, value_2);
                OLED_ShowString(0, 3, "LED3: ");
                OLED_ShowNum(40, 3, value_3);
                OLED_ShowString(0, 4, "LED4: ");
                OLED_ShowNum(40, 4, value_4);
                OLED_ShowString(0, 1, "->");  // Initial cursor position on LED1
            } 
            break;
            
        case 2: // Debug MPU
            Update_MPU(angle);
            break;
            
        case 3: // Debug LED
            first_entry = 1;  // Reset trạng thái để vẽ lại toàn bộ màn hình
            Update_Led(value_1, value_2, value_3, value_4, 0, 0);
            break;
    }
    
    Reset_Status();
    
    // Wait in this menu until button 3 is held for 1 second or selection is made
    while(1) {
       Check_Status();
        uint32_t current_time = millis();
        
        // Handle MPU calibration menu
        if(menu == 0) {
            // Xử lý điều hướng cho 4 lựa chọn
            Handle_Calibration_Navigation(&calib_cursor_pos, 4);

            // Kiểm tra vị trí con trỏ có thay đổi không
            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Xóa các vị trí con trỏ cũ
                OLED_ShowString(0, 0, "   ");
                OLED_ShowString(0, 1, "   ");
                OLED_ShowString(0, 2, "   ");
                OLED_ShowString(0, 3, "   ");
                
                // Vẽ con trỏ ở vị trí mới
                OLED_ShowString(0, calib_cursor_pos, "->");
                
                // Cập nhật vị trí đã biết cuối cùng
                last_calib_cursor_pos = calib_cursor_pos;
            }

            // Cập nhật giá trị góc hiện tại liên tục
            if(current_time - last_update_time >= UPDATE_INTERVAL) {
                last_update_time = current_time;
                OLED_ClearArea(75, 0, 5);
                OLED_ShowFloat(75, 0, angle);
            }

            // Xử lý lựa chọn
            if(flag_stt.flag_3) {
                if(calib_cursor_pos == 0) {
                    // "Angle" được chọn - Lưu hoặc xóa góc
                    if(calibrated_angle != 0) {
                        // Đã lưu trước đó, xóa giá trị
                        calibrated_angle = 0;
                        
                        // Hiển thị xác nhận
                        OLED_Clear();
                        OLED_ShowString(10, 2, "Angle Cleared!");
                        delay_ms(1000);
                    } else {
                        // Chưa lưu, lưu giá trị mới
                        calibrated_angle = angle;
                        
                        // Hiển thị xác nhận
                        OLED_Clear();
                        OLED_ShowString(10, 2, "Angle Saved!");
                        delay_ms(1000);
                    }
                    
                    // Vẽ lại menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    OLED_ShowString(0, 0, "->");
                    
                    calib_cursor_pos = 0;
                    last_calib_cursor_pos = 0;
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 1) {
                    // "Show Value" được chọn - Hiển thị giá trị đã lưu
                    OLED_Clear();
                    OLED_ShowString(0, 0, "Angle:");
                    OLED_ShowFloat(55, 0, calibrated_angle);
                    
                    // Reset biến cho việc kiểm tra nhấn giữ
                    uint32_t show_press_start = 0;
                    uint8_t show_is_button_held = 0;
                    
                    // Vòng lặp chờ người dùng nhấn giữ nút
                    while(1) {
                        Check_Status();
                        uint32_t current_time = millis();
                        
                        // Kiểm tra nếu người dùng nhấn giữ nút 3
                        if(Check_Long_Press(stt_3, current_time, &show_press_start, &show_is_button_held)) {
                            break; // Thoát khỏi vòng lặp và quay lại menu trước đó
                        }
                    }
                    
                    // Vẽ lại menu
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    OLED_ShowString(0, 1, "->");  // Đặt con trỏ ở Show Value
                    
                    calib_cursor_pos = 1;
                    last_calib_cursor_pos = 1;
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 2) {
                    // "Clear Value" được chọn - Xóa giá trị nhưng không quay về menu chính
                    calibrated_angle = 0;
                    
                    // Hiển thị xác nhận
                    OLED_Clear();
                    OLED_ShowString(10, 2, "Angle Cleared!");
                    delay_ms(1000);
                    
                    // Vẽ lại menu Calib MPU thay vì thoát về menu chính
                    OLED_Clear();
                    OLED_ShowString(25, 0, "Angle: ");
                    OLED_ShowFloat(75, 0, angle);
                    OLED_ShowString(25, 1, "Show Value");
                    OLED_ShowString(25, 2, "Clear Value");
                    OLED_ShowString(25, 3, "Exit");
                    
                    // Đặt con trỏ ở vị trí Clear Value
                    OLED_ShowString(0, 2, "->");
                    
                    calib_cursor_pos = 2;
                    last_calib_cursor_pos = 2;
                    flag_stt.flag_3 = 0;
                    
                    // Không có break ở đây để tiếp tục ở trong menu này
                }
                else if(calib_cursor_pos == 3) {
                    // "Exit" được chọn - Quay về menu chính
                    break;
                }
            }
        }
        
        // Handle LED calibration menu
        if(menu == 1) {
            // Di chuyển giữa 7 mục: 4 LEDs, Show Value, Clear Value, Exit
            Handle_Calibration_Navigation(&calib_cursor_pos, 7);

            // Xác định vị trí bắt đầu hiển thị dựa trên vị trí con trỏ
            uint8_t start_pos = 0;
            if(calib_cursor_pos > 2) {
                start_pos = calib_cursor_pos - 2;
            }
            if(start_pos > 3) {  // Đảm bảo không vượt quá giới hạn
                start_pos = 3;
            }

            // Chỉ vẽ lại menu nếu vị trí con trỏ thay đổi
            if(calib_cursor_pos != last_calib_cursor_pos) {
                OLED_Clear();
                
                // Hiển thị 4 mục menu từ vị trí bắt đầu
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 7; i++) {
                    uint8_t item = start_pos + i;
                    if(item < 4) {
                        // Hiển thị 4 LED
                        OLED_ShowString(25, i, "LED");
                        //OLED_ShowNum(60, i, item+1);
                        OLED_ShowString(50, i, ": ");
                        
                        // Hiển thị giá trị tương ứng
                        uint16_t value;
                        switch(item) {
                            case 0: value = value_1; break;
                            case 1: value = value_2; break;
                            case 2: value = value_3; break;
                            case 3: value = value_4; break;
                            default: value = 0;
                        }
                        OLED_ShowNum(60, i, value);
                    }
                    else if(item == 4) {
                        OLED_ShowString(25, i, "Show Value");
                    }
                    else if(item == 5) {
                        OLED_ShowString(25, i, "Clear Value");
                    }
                    else if(item == 6) {
                        OLED_ShowString(25, i, "Exit");
                    }
                }
                
                // Hiển thị con trỏ tại vị trí tương ứng
                uint8_t cursor_y = calib_cursor_pos - start_pos;
                OLED_ShowString(0, cursor_y, "->");
                
                last_calib_cursor_pos = calib_cursor_pos;
            }
            
            // Cập nhật giá trị LED theo thời gian thực
            if(current_time - last_update_time >= UPDATE_INTERVAL) {
                last_update_time = current_time;
                
                // Chỉ cập nhật các LED hiển thị trên màn hình
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 4; i++) {
                    uint8_t led_idx = start_pos + i;
                    uint16_t value;
                    
                    switch(led_idx) {
                        case 0: value = value_1; break;
                        case 1: value = value_2; break;
                        case 2: value = value_3; break;
                        case 3: value = value_4; break;
                        default: value = 0;
                    }
                    
                    OLED_ClearArea(60, i, 5);
                    OLED_ShowNum(60, i, value);
                }
            }
            
            // Sửa phần xử lý lựa chọn trong menu Calib LED
            if(flag_stt.flag_3) {
                if(calib_cursor_pos < 4) {
                    // Kiểm tra xem LED này đã được lưu trước đó chưa
                    uint8_t current_led = calib_cursor_pos;
                    
                    if(calibrated_leds[current_led] != 0) {
                        // Đã lưu trước đó, xóa giá trị đã lưu
                        calibrated_leds[current_led] = 0;
                        
                        // Hiển thị thông báo xóa
                        OLED_Clear();
                        OLED_ShowString(10, 1, "LED");
                        OLED_ShowNum(35, 1, current_led+1);
                        OLED_ShowString(40, 1, " Cleared!");
                        delay_ms(1000);
                    } else {
                        // Chưa lưu, lưu giá trị mới
                        uint16_t value_to_save = 0;
                        switch(current_led) {
                            case 0: value_to_save = value_1; break;
                            case 1: value_to_save = value_2; break;
                            case 2: value_to_save = value_3; break;
                            case 3: value_to_save = value_4; break;
                        }
                        
                        calibrated_leds[current_led] = value_to_save;
                        
                        // Hiển thị xác nhận lưu
                        OLED_Clear();
                        OLED_ShowString(10, 1, "LED");
                        OLED_ShowNum(35, 1, current_led+1);
                        OLED_ShowString(40, 1, " Saved!");
                        delay_ms(1000);
                    }
                }
                // Phần còn lại của code giữ nguyên
                else if(calib_cursor_pos == 4) {
                    // Show Value - Hiển thị các giá trị đã lưu
                    OLED_Clear();
                    for(uint8_t i = 0; i < 4; i++) {
                        OLED_ShowString(15, i, "LED");
                        //OLED_ShowNum(50, i, i+1);
                        OLED_ShowString(40, i, ": ");
                        OLED_ShowNum(50, i, calibrated_leds[i]);
                    }
                    
                    // Reset biến cho việc kiểm tra nhấn giữ
                    uint32_t show_press_start = 0;
                    uint8_t show_is_button_held = 0;
                    
                    // Vòng lặp chờ người dùng nhấn giữ nút
                    while(1) {
                        Check_Status();
                        uint32_t current_time = millis();
                        
                        // Kiểm tra nếu người dùng nhấn giữ nút 3
                        if(Check_Long_Press(stt_3, current_time, &show_press_start, &show_is_button_held)) {
                            break; // Thoát khỏi vòng lặp và quay lại menu trước đó
                        }
                    }
                }
                else if(calib_cursor_pos == 5) {
                    // Clear Value - Xóa tất cả giá trị LED đã lưu
                    for(int i = 0; i < 4; i++) {
                        calibrated_leds[i] = 0;
                    }
                    
                    // Hiển thị xác nhận
                    OLED_Clear();
                    OLED_ShowString(10, 2, "Values Cleared!");
                    delay_ms(1000);
                }
                else if(calib_cursor_pos == 6) {
                    // Exit - Quay về menu chính
                    break;
                }
                
                // Vẽ lại menu
                OLED_Clear();
                uint8_t start_pos = 0;
                if(calib_cursor_pos > 2) {
                    start_pos = calib_cursor_pos - 2;
                }
                if(start_pos > 3) {
                    start_pos = 3;
                }
                
                for(uint8_t i = 0; i < 4 && (start_pos + i) < 7; i++) {
                    uint8_t item = start_pos + i;
                    if(item < 4) {
                        OLED_ShowString(25, i, "LED");
                       //OLED_ShowNum(40, i, item+1);
                        OLED_ShowString(50, i, ": ");
                        
                        uint16_t value;
                        switch(item) {
                            case 0: value = value_1; break;
                            case 1: value = value_2; break;
                            case 2: value = value_3; break;
                            case 3: value = value_4; break;
                            default: value = 0;
                        }
                        OLED_ShowNum(60, i, value);
                    }
                    else if(item == 4) {
                        OLED_ShowString(25, i, "Show Value");
                    }
                    else if(item == 5) {
                        OLED_ShowString(25, i, "Clear Value");
                    }
                    else if(item == 6) {
                        OLED_ShowString(25, i, "Exit");
                    }
                }
                
                uint8_t cursor_y = calib_cursor_pos - start_pos;
                OLED_ShowString(0, cursor_y, "->");
                
                flag_stt.flag_3 = 0;
            }
        }
        
        // Handle Debug LED menu
        if(menu == 3) {
            static uint8_t show_calibrated = 0; // 0: current values, 1: calibrated values
            
            // Toggle between current and calibrated values using up/down buttons
            if(flag_stt.flag_1 || flag_stt.flag_2) {
                show_calibrated = !show_calibrated;
                Update_Led(value_1, value_2, value_3, value_4, show_calibrated, 0);
                flag_stt.flag_1 = 0;
                flag_stt.flag_2 = 0;
            }
            
            // Update values periodically (only for current values page)
            if(current_time - last_update_time >= UPDATE_INTERVAL && !show_calibrated) {
                last_update_time = current_time;
                
                // Chỉ cập nhật giá trị số, không vẽ lại toàn bộ màn hình
                OLED_ClearArea(60, 1, 5);
                OLED_ShowNum(60, 1, value_1);
                
                OLED_ClearArea(60, 2, 5);
                OLED_ShowNum(60, 2, value_2);
                
                OLED_ClearArea(60, 3, 5);
                OLED_ShowNum(60, 3, value_3);
                
                OLED_ClearArea(60, 4, 5);
                OLED_ShowNum(60, 4, value_4);
            }
            
            // Long press to exit
            if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                break; // Exit menu
            }
        }
        
        // Long press detection for button 3 (only for debug menus)
        if((menu == 2 || menu == 3) && 
           Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
            break; // Exit the menu
        }
        
        // Sửa phần này để không cập nhật lại menu Debug LED
        if((menu == 2 || menu == 3) && (current_time - last_update_time >= UPDATE_INTERVAL)) {
            last_update_time = current_time;
            
            if(menu == 2) { // Debug MPU
                OLED_ClearArea(70, 2, 6);
                OLED_ShowFloat(70, 2, angle);
            } 
        }
    }
    
    // Handle transition back to main menu
    OLED_Clear();
    Reset_Status();
}

// Modified Update_Led function for Debug LED
void Update_Led(uint16_t value_1, uint16_t value_2, uint16_t value_3, uint16_t value_4, 
                uint8_t show_calibrated, uint8_t led_cursor_pos)
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
        OLED_ShowNum(60, 0, value_1);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, value_2);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, value_3);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, value_4);
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
void Update_MPU(float value)
{
    static uint8_t first_draw = 1;
    
    // Luôn vẽ lại toàn bộ nội dung khi vào menu Debug MPU
    OLED_ShowString(25, 0, "MPU Debug");
    OLED_ShowString(0, 1, "Calibrated:");
    OLED_ShowString(0, 2, "Current:");
    
    // Cập nhật cả hai giá trị
    OLED_ClearArea(90, 1, 6);  // Xóa vùng hiển thị giá trị đã hiệu chuẩn
    OLED_ShowFloat(90, 1, calibrated_angle);
    
    OLED_ClearArea(70, 2, 6);  // Xóa vùng hiển thị giá trị hiện tại
    OLED_ShowFloat(70, 2, value);
}