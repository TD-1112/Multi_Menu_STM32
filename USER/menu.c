#include "menu.h"

//------------------------------------
// BIẾN TOÀN CỤC
//------------------------------------
float calibrated_angle = 0;
uint16_t calibrated_leds[4];
static uint8_t first_entry = 1;  // Biến để kiểm tra lần đầu vào menu

// Giá trị cảm biến toàn cục
float g_angle;
uint16_t g_value_1;
uint16_t g_value_2;
uint16_t g_value_3;
uint16_t g_value_4;

//------------------------------------
// CÁC HÀM TIỆN ÍCH HIỂN THỊ
//------------------------------------

/**
 * @brief Vẽ menu hiệu chuẩn MPU
 * 
 * @param current_angle - Góc hiện tại
 * @param saved_angle - Góc đã lưu
 * @param cursor_pos - Vị trí con trỏ (0-2)
 */
void Draw_MPU_Calibration_Menu(float current_angle, float saved_angle, uint8_t cursor_pos)
{
    OLED_Clear();
    OLED_ShowString(25, 0, "Angle: ");
    OLED_ShowFloat(75, 0, current_angle);
    OLED_ShowString(25, 1, "Saved:");
    OLED_ShowFloat(75, 1, saved_angle);
    OLED_ShowString(25, 2, "Exit");
    OLED_ShowString(0, cursor_pos, "->");
}

/**
 * @brief Vẽ giá trị hiệu chuẩn LED
 */
void Draw_LED_Calibration_Values(uint16_t val_fr, uint16_t val_fl, uint16_t val_r, uint16_t val_l, uint8_t cursor_pos)
{
    OLED_Clear();
    OLED_ShowString(25, 0, "FR:");
    OLED_ShowNum(70, 0, val_fr);
    OLED_ShowString(25, 2, "R:");
    OLED_ShowNum(70, 2, val_r);
    OLED_ShowString(25, 3, "L:");
    OLED_ShowNum(70, 3, val_l);
    OLED_ShowString(25, 1, "FL:");
    OLED_ShowNum(70, 1, val_fl);
    
    // Vẽ con trỏ nếu cursor_pos < 4
    if(cursor_pos < 4) {
        OLED_ShowString(0, cursor_pos, "->");
    }
}

/**
 * @brief Vẽ menu tùy chọn hiệu chuẩn LED
 */
void Draw_LED_Calibration_Options(uint8_t rel_cursor_pos)
{
    OLED_Clear();
    OLED_ShowString(25, 0, "Show Values");
    OLED_ShowString(25, 1, "Clear All");
    OLED_ShowString(25, 2, "Exit");
    
    // Vẽ con trỏ tại vị trí
    OLED_ShowString(0, rel_cursor_pos, "->");
}

/**
 * @brief Hiển thị thông báo
 */
void Show_Message(const char* message, uint16_t delay_ms_time)
{
    OLED_Clear();
    OLED_ShowString(10, 2, message);
    delay_ms(delay_ms_time);
}

/**
 * @brief Hiển thị thông báo về LED
 */
void Show_LED_Message(uint8_t led_idx, const char* action, uint16_t delay_ms_time)
{
    OLED_Clear();
    OLED_ShowString(10, 2, "LED");
    OLED_ShowNum(40, 2, led_idx + 1);
    OLED_ShowString(50, 2, action);
    delay_ms(delay_ms_time);
}

/**
 * @brief Vẽ con trỏ trên menu
 * 
 * @param line_count - Số dòng trong menu
 * @param cursor_pos - Vị trí con trỏ hiện tại
 */
void Draw_Cursor(uint8_t line_count, uint8_t cursor_pos)
{
    // Xóa tất cả vị trí con trỏ
    for(uint8_t i = 0; i < line_count; i++) {
        OLED_ShowString(0, i, "  ");
    }
    
    // Vẽ con trỏ ở vị trí mới
    OLED_ShowString(0, cursor_pos, "->");
}

/**
 * @brief Xóa một dòng trên màn hình OLED
 * 
 * @param line - Dòng cần xóa (0-3)
 */
void OLED_ClearLine(uint8_t line)
{
    OLED_ShowString(0, line, "                    "); // 20 spaces để xóa dòng
}

/**
 * @brief Xóa một vùng cụ thể trên màn hình OLED
 * 
 * @param x - Tọa độ X của vùng cần xóa
 * @param y - Tọa độ Y của vùng cần xóa
 * @param width - Độ rộng của vùng cần xóa (tính theo ký tự)
 */
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width)
{
    char spaces[21] = "                    "; // 20 spaces
    spaces[width] = '\0'; // Cắt bớt độ rộng
    OLED_ShowString(x, y, spaces);
}

//------------------------------------
// CÁC HÀM ĐIỀU HƯỚNG
//------------------------------------

/**
 * @brief Điều hướng trong menu LED debug
 * 
 * @param led_cursor_pos - Vị trí con trỏ (0-5)
 * @param led_page - Trang hiện tại (0 hoặc 1)
 */
void Handle_LED_Navigation(uint8_t *led_cursor_pos, uint8_t *led_page)
{
    // Xử lý nút Down (flag_stt.flag_1)
    if(flag_stt.flag_1) {
        // Di chuyển con trỏ xuống
        (*led_cursor_pos)++;
        if(*led_cursor_pos >= LED_MAX) {
            *led_cursor_pos = 0; // Quay về đầu
        }
        
        // Kiểm tra xem có cần chuyển trang không
        uint8_t new_page = *led_cursor_pos / LEDS_PER_PAGE;
        if(new_page != *led_page) {
            *led_page = new_page;
            OLED_Clear(); // Xóa màn hình khi chuyển trang
        }
        
        // Cập nhật hiển thị với vị trí con trỏ mới
        Update_Led(0, *led_cursor_pos);
        flag_stt.flag_1 = 0;
    }
    
    // Xử lý nút Up (flag_stt.flag_2)
    if(flag_stt.flag_2) {
        // Di chuyển con trỏ lên
        if(*led_cursor_pos == 0) {
            *led_cursor_pos = LED_MAX - 1; // Quay về cuối
        } else {
            (*led_cursor_pos)--;
        }
        
        // Kiểm tra xem có cần chuyển trang không
        uint8_t new_page = *led_cursor_pos / LEDS_PER_PAGE;
        if(new_page != *led_page) {
            *led_page = new_page;
            OLED_Clear(); // Xóa màn hình khi chuyển trang
        }
        
        // Cập nhật hiển thị với vị trí con trỏ mới
        Update_Led(0, *led_cursor_pos);
        flag_stt.flag_2 = 0;
    }
    
    // Xử lý nút chọn (flag_stt.flag_3)
    if(flag_stt.flag_3) {
        // Bạn có thể thêm logic điều khiển LED ở đây
        // LED_Toggle(*led_cursor_pos + 1);  
        
        flag_stt.flag_3 = 0;
    }
}

/**
 * @brief Điều hướng trong menu hiệu chuẩn
 * 
 * @param calib_cursor_pos - Vị trí con trỏ (0-max_pos-1)
 * @param max_pos - Số mục menu tối đa
 */
void Handle_Calibration_Navigation(uint8_t *calib_cursor_pos, uint8_t max_pos)
{
    // Xử lý nút Down (flag_stt.flag_1)
    if(flag_stt.flag_1) {
        (*calib_cursor_pos)++;
        if(*calib_cursor_pos >= max_pos) {
            *calib_cursor_pos = 0; // Quay vòng
        }
        flag_stt.flag_1 = 0;
    }
    
    // Xử lý nút Up (flag_stt.flag_2)
    if(flag_stt.flag_2) {
        if(*calib_cursor_pos == 0) {
            *calib_cursor_pos = max_pos - 1; // Quay vòng
        } else {
            (*calib_cursor_pos)--;
        }
        flag_stt.flag_2 = 0;
    }
}

//------------------------------------
// CÁC HÀM MENU CHÍNH
//------------------------------------

/**
 * @brief Menu chính
 */
void Main_Menu(void)   
{
    Check_Status(); // Kiểm tra trạng thái nút nhấn
    static uint8_t cursor_pos = 0; // Đặt con trỏ ở vị trí đầu tiên
    static uint8_t last_drawn_pos = 255; // Vị trí đã vẽ trước đó
    uint8_t menu_items = 4; // Số lượng mục menu
    
    // Kiểm tra xem có nhấn nút lên hay xuống không
    if(flag_stt.flag_1)
    {
        cursor_pos++; // Tăng vị trí con trỏ
        if(cursor_pos >= menu_items)  // Nếu vượt quá số lượng mục menu thì quay về đầu danh sách
            cursor_pos = 0;     // Vòng lại đầu danh sách
        
        flag_stt.flag_1 = 0; // Reset cờ để không bị lặp lại
        delay_ms(150); // Thêm độ trễ cho di chuyển mượt mà
    }
    
    if(flag_stt.flag_2)
    {
        if(cursor_pos == 0) // Nếu đang ở vị trí đầu tiên thì quay về cuối danh sách
            cursor_pos = menu_items - 1;  // Vòng lại cuối danh sách
        else
            cursor_pos--; // Giảm vị trí con trỏ
        
        flag_stt.flag_2 = 0;
        delay_ms(150); // Thêm độ trễ cho di chuyển mượt mà
    }
    
    // Chỉ vẽ lại menu khi lần đầu vào menu hoặc có thay đổi vị trí con trỏ
    static uint8_t menu_initialized = 0; // Biến này dùng để kiểm tra xem menu đã được khởi tạo chưa
    if(!menu_initialized) 
    {
        OLED_ShowString(20, 0, "Calib MPU");
        OLED_ShowString(20, 1, "Calib Led");
        OLED_ShowString(20, 2, "Debug Led");
        OLED_ShowString(20, 3, "Exit");
        menu_initialized = 1; // Đánh dấu menu đã được khởi tạo
    }
    
    // Chỉ vẽ lại con trỏ khi có thay đổi vị trí
    if(cursor_pos != last_drawn_pos) {
        Draw_Cursor(menu_items, cursor_pos);
        last_drawn_pos = cursor_pos; // Cập nhật vị trí đã vẽ trước đó
    }
    
    if(flag_stt.flag_3)
    {
        // Cập nhật giá trị cảm biến trước khi vào menu con
        get_value();
        // Gọi Select_Menu với vị trí con trỏ hiện tại và các biến toàn cục
        Select_Menu(cursor_pos);
        // Buộc vẽ lại menu khi quay lại
        menu_initialized = 0; // Đặt lại biến khởi tạo menu để vẽ lại menu chính
        last_drawn_pos = 255; // Đặt lại vị trí đã vẽ trước đó để vẽ lại con trỏ
        flag_stt.flag_3 = 0; // Reset cờ sau khi xử lý
    }
}

/**
 * @brief Chọn menu con
 * 
 * @param menu - Số thứ tự menu (0-3)
 */
void Select_Menu(uint8_t menu)
{
    uint32_t button_press_start = 0;  // Cho phát hiện nhấn giữ
    uint8_t is_button_held = 0; // Cho phát hiện nhấn giữ
    
    // Cho việc cập nhật giá trị thời gian thực trong màn hình debug
    uint32_t last_update_time = 0;
    
    // Các biến cho menu Debug LED
    uint8_t led_cursor_pos = 0;       // Vị trí 0-5 (tuyệt đối)
    uint8_t led_page = 0;             // Trang hiện tại (0 hoặc 1)
    
    // Các biến cho menu hiệu chuẩn
    uint8_t calib_cursor_pos = 0;     // Vị trí cho lựa chọn yes/no hoặc màn hình giá trị
    uint8_t calib_page = 0;           // Cho hiệu chuẩn LED (0=giá trị, 1=yes/no)
    uint8_t last_calib_cursor_pos = 255; // Cho vẽ lại hiệu quả
    
    // Xóa màn hình và hiển thị menu đã chọn
    OLED_Clear();
    
    switch(menu) {
        case 0: // Calib MPU
            get_value();
            Draw_MPU_Calibration_Menu(g_angle, calibrated_angle, 0);
            break;
            
        case 1: // Calib LED
            if(calib_page == 0) {
                // Giá trị LED hiện tại
                Draw_LED_Calibration_Values(g_value_1, g_value_4, g_value_2, g_value_3, 0);
            } 
            break;
            
        case 2: // Debug LED
            first_entry = 1;  // Reset trạng thái 
            Update_Led(0, 0);
            break;
        case 3:
            OLED_Clear();
            break;
    }
    
    Reset_Status();
    
    /*  
        Vòng while này dùng để xử lý các sự kiện trong menu đã chọn
        Chương trình sẽ chạy liên tục cho đến khi có sự kiện thoát menu
    */ 
    while(1) {
        Check_Status();
        uint32_t current_time = millis();
        
        // Cập nhật tất cả giá trị cảm biến định kỳ cho tất cả menu
        if(current_time - last_update_time >= UPDATE_INTERVAL) 
        {
            last_update_time = current_time;
            
            // Cập nhật tất cả giá trị toàn cục cùng một lúc
            get_value();
            
            // Cập nhật hiển thị menu hiệu chuẩn MPU
            if(menu == 0) {
                OLED_ClearArea(75, 0, 8);
                OLED_ShowFloat(75, 0, g_angle);
                OLED_ClearArea(75, 1, 8);
                OLED_ShowFloat(75, 1, calibrated_angle);
            } 
        }
        
        // Xử lý menu hiệu chuẩn MPU
        if(menu == 0) {
            // Xử lý điều hướng menu hiệu chuẩn MPU với chỉ 3 tùy chọn
            Handle_Calibration_Navigation(&calib_cursor_pos, 3);

            if(calib_cursor_pos != last_calib_cursor_pos) {
                // Xóa các vị trí con trỏ cũ
                Draw_Cursor(3, calib_cursor_pos);
                last_calib_cursor_pos = calib_cursor_pos;
            }

            // Xử lý lựa chọn
            if(flag_stt.flag_3) {
                if(calib_cursor_pos == 0) {
                    // Lựa chọn "Angle" - lưu góc
                    // Lấy giá trị mới nhất trước khi lưu
                    get_value();
                    calibrated_angle = g_angle;
                    Show_Message("Angle Saved!", 1000);
                    Draw_MPU_Calibration_Menu(g_angle, calibrated_angle, calib_cursor_pos);
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 1) {
                    // Lựa chọn "Saved" - xóa góc đã lưu
                    calibrated_angle = 0;
                    Show_Message("Angle Cleared!", 1000);
                    Draw_MPU_Calibration_Menu(g_angle, calibrated_angle, calib_cursor_pos);
                    flag_stt.flag_3 = 0;
                }
                else if(calib_cursor_pos == 2) {
                    // Lựa chọn "Exit"
                    break;
                }
            }
        }
        
        // Xử lý menu hiệu chuẩn LED
        else if(menu == 1) {
            // Khởi tạo: Hiển thị giá trị LED lần đầu vào
            Draw_LED_Calibration_Values(g_value_1, g_value_4, g_value_2, g_value_3, 0);
            
            // Xử lý menu hiệu chuẩn LED
            calib_cursor_pos = 0;
            last_calib_cursor_pos = 255; // Buộc vẽ lại ban đầu
            
            while(1) {
                Check_Status();
                uint32_t current_time = millis();
                
                // Cập nhật giá trị định kỳ
                if(current_time - last_update_time >= UPDATE_INTERVAL) {
                    last_update_time = current_time;
                    get_value();
                    
                    // Chỉ cập nhật giá trị LED hiển thị nếu đang ở trang đầu
                    if(calib_cursor_pos < 4) {
                        OLED_ClearArea(70, 0, 5);
                        OLED_ShowNum(70, 0, g_value_1);
                        OLED_ClearArea(70, 2, 5);
                        OLED_ShowNum(70, 2, g_value_2);
                        OLED_ClearArea(70, 3, 5);
                        OLED_ShowNum(70, 3, g_value_3);
                        OLED_ClearArea(70, 1, 5);
                        OLED_ShowNum(70, 1, g_value_4);
                    }
                }
                
                // Điều hướng
                if(flag_stt.flag_1) {
                    flag_stt.flag_1 = 0;
                    calib_cursor_pos++;
                    if(calib_cursor_pos >= 7) {
                        calib_cursor_pos = 0;
                    }
                    
                    // Xử lý chuyển trang
                    if((calib_cursor_pos == 4 && last_calib_cursor_pos < 4) || 
                       (calib_cursor_pos < 4 && last_calib_cursor_pos >= 4)) {
                        last_calib_cursor_pos = 255; // Buộc vẽ lại
                    }
                }
                
                if(flag_stt.flag_2) {
                    flag_stt.flag_2 = 0;
                    if(calib_cursor_pos == 0) {
                        calib_cursor_pos = 6;
                    } else {
                        calib_cursor_pos--;
                    }
                    
                    // Xử lý chuyển trang
                    if((calib_cursor_pos == 3 && last_calib_cursor_pos >= 4) || 
                       (calib_cursor_pos >= 4 && last_calib_cursor_pos < 4)) {
                        last_calib_cursor_pos = 255; // Buộc vẽ lại
                    }
                }
                
                // Vẽ lại nếu vị trí con trỏ thay đổi
                if(calib_cursor_pos != last_calib_cursor_pos) {
                    // Vẽ trang phù hợp
                    if(calib_cursor_pos < 4) {
                        // Trang giá trị LED
                        Draw_LED_Calibration_Values(g_value_1, g_value_4, g_value_2, g_value_3, calib_cursor_pos);
                    } else {
                        // Trang tùy chọn menu
                        Draw_LED_Calibration_Options(calib_cursor_pos - 4);
                    }
                    
                    last_calib_cursor_pos = calib_cursor_pos;
                }
                
                // Xử lý lựa chọn
                if(flag_stt.flag_3) {
                    flag_stt.flag_3 = 0;
                    
                    if(calib_cursor_pos < 4) {
                        // Lựa chọn LED - lưu hoặc xóa LED này
                        uint8_t led_idx = calib_cursor_pos;
                        
                        if(calibrated_leds[led_idx] != 0) {
                            // Đã hiệu chuẩn, xóa nó
                            calibrated_leds[led_idx] = 0;
                            Show_LED_Message(led_idx, " Cleared", 1000);
                        } else {
                            // Lưu giá trị hiện tại
                            get_value();
                            uint16_t value = 0;
                            switch(led_idx) {
                                case 0: value = g_value_1; break; //FR_S
                                case 1: value = g_value_4; break; //FL_s
                                case 2: value = g_value_2; break; // R_S
                                case 3: value = g_value_3; break; // L_S
                            }
                            calibrated_leds[led_idx] = value;
                            Show_LED_Message(led_idx, " Saved", 1000);
                        }
                        
                        // Vẽ lại trang hiện tại
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 4) {
                        // Tùy chọn "Show Values"
                        OLED_Clear();
                        OLED_ShowString(0, 0, "FR:");
                        OLED_ShowNum(40, 0, calibrated_leds[0]);
                        OLED_ShowString(0, 2, "R:");
                        OLED_ShowNum(40, 2, calibrated_leds[2]);
                        OLED_ShowString(0, 3, "L:");
                        OLED_ShowNum(40, 3, calibrated_leds[3]);
                        OLED_ShowString(0, 1, "FL:");
                        OLED_ShowNum(40, 1, calibrated_leds[1]);
                        
                        // Đợi nhấn giữ lâu để thoát
                        uint32_t show_press_start = 0;
                        uint8_t show_is_button_held = 0;
                        
                        while(1) {
                            Check_Status();
                            uint32_t curr_time = millis();
                            // Kiểm tra nhấn nút và giữ
                            if(stt_3 == 0) { // Nút được nhấn
                                if(!show_is_button_held) {
                                    // Nút mới được nhấn, ghi lại thời gian bắt đầu
                                    show_press_start = curr_time;
                                    show_is_button_held = 1;
                                } else {
                                    // Kiểm tra xem nút đã được giữ đủ lâu chưa
                                    if(curr_time - show_press_start >= LONG_PRESS_DURATION) {
                                        // Phát hiện nhấn giữ lâu, thoát vòng lặp
                                        break;
                                    }
                                }
                            } else {
                                // Nút thả ra
                                show_is_button_held = 0;
                            }
                            
                            // Độ trễ cần thiết để tránh tải CPU quá mức
                            delay_ms(10);
                        }
                        
                        // Buộc vẽ lại khi quay lại
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 5) {
                        // Tùy chọn "Clear All"
                        for(uint8_t i = 0; i < 4; i++) {
                            calibrated_leds[i] = 0;
                        }
                        
                        Show_Message("Values Cleared", 1000);
                        
                        // Buộc vẽ lại khi quay lại
                        last_calib_cursor_pos = 255;
                    }
                    else if(calib_cursor_pos == 6) {
                        // Thoát - thoát khỏi vòng lặp menu
                        break;
                    }
                }
                
                // Kiểm tra nhấn giữ lâu để thoát (chỉ trên các trang LED)
                if(calib_cursor_pos < 4) {
                    if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                        break;
                    }
                }
            }
            break;
        }
        
        // Xử lý menu Debug LED 
        else if(menu == 2) {
            static uint8_t debug_page = 0; // 0 = giá trị hiện tại, 1 = giá trị đã lưu
            
            // Khởi tạo lần đầu vào
            OLED_Clear();
            // Buộc vẽ lại
            first_entry = 1;
            Update_Led(debug_page, 0);
            
            while(1) {
                Check_Status();
                uint32_t current_time = millis();
                
                // Cập nhật giá trị định kỳ (chỉ cho trang giá trị hiện tại)
                if(current_time - last_update_time >= UPDATE_INTERVAL && debug_page == 0) {
                    last_update_time = current_time;
                    get_value();
                    Update_Led(debug_page, 0);
                }
                
                // Chuyển đổi giữa các trang
                if(flag_stt.flag_1 || flag_stt.flag_2) {
                    debug_page = !debug_page;
                    
                    // Buộc vẽ lại đầy đủ giá trị với trang mới
                    first_entry = 1;
                    Update_Led(debug_page, 0);
                    
                    flag_stt.flag_1 = 0;
                    flag_stt.flag_2 = 0;
                }
                
                // Kiểm tra nhấn giữ lâu để thoát
                if(Check_Long_Press(stt_3, current_time, &button_press_start, &is_button_held)) {
                    break;
                }
                
                // Thêm độ trễ nhỏ để tránh tải CPU quá mức
                delay_ms(10);
            }
            break;
        }
    }
    
    // Xử lý chuyển đổi trở lại menu chính
    OLED_Clear();
    Reset_Status();
}

/**
 * @brief Cập nhật giá trị LED trong menu Debug LED
 * 
 * @param show_calibrated - 0: Hiển thị giá trị hiện tại, 1: Hiển thị giá trị đã hiệu chuẩn
 * @param led_cursor_pos - Vị trí con trỏ LED (0-5)
 */
void Update_Led(uint8_t show_calibrated, uint8_t led_cursor_pos)
{
    // Đặt lại biến tĩnh nếu đây là lần vào menu Debug LED (led_cursor_pos == 0)
    static uint8_t current_mode = 255;

    // Luôn vẽ lại nhãn khi lần đầu vào menu từ Select_Menu
    // Hoặc khi chuyển chế độ hiển thị
    if(first_entry || current_mode != show_calibrated) 
    {
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
    if(!show_calibrated) 
    {
        // Hiển thị giá trị hiện tại
        OLED_ClearArea(60, 0, 5);
        OLED_ShowNum(60, 0, g_value_1);
        
        OLED_ClearArea(60, 2, 5);
        OLED_ShowNum(60, 2, g_value_2);
        
        OLED_ClearArea(60, 3, 5);
        OLED_ShowNum(60, 3, g_value_3);
        
        OLED_ClearArea(60, 1, 5);
        OLED_ShowNum(60, 1, g_value_4);
    }
    else 
    {
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

/**
 * @brief Lấy giá trị từ các cảm biến
 */
void get_value(void)
{
    g_angle = MPU6050.Get_Yaw(0);
    g_value_1 = IRSensor_data[FR_S];
    g_value_2 = IRSensor_data[R_S];
    g_value_3 = IRSensor_data[L_S];
    g_value_4 = IRSensor_data[FL_S];
}