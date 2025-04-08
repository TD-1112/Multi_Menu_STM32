#ifndef __MENU__
#define __MENU__

#include "define.h"
#include "button.h"

// Các định nghĩa cho menu
#define UPDATE_INTERVAL      0     // Display update interval in milliseconds
#define LED_MAX              6     // Total number of LEDs
#define LEDS_PER_PAGE        4     // Number of LEDs shown per page

// Biến phục vụ hiệu chuẩn
extern float calibrated_angle;
extern uint16_t calibrated_leds[4];

// Biến giá trị cảm biến toàn cục
extern float g_angle;
extern uint16_t g_value_1;
extern uint16_t g_value_2;
extern uint16_t g_value_3;
extern uint16_t g_value_4;

// Các hàm menu chính
void Main_Menu(void);
void Select_Menu(uint8_t menu);

// Các hàm hiển thị menu
void Draw_MPU_Calibration_Menu(float current_angle, float saved_angle, uint8_t cursor_pos);
void Draw_LED_Calibration_Values(uint16_t val_fr, uint16_t val_fl, uint16_t val_r, uint16_t val_l, uint8_t cursor_pos); 
void Draw_LED_Calibration_Options(uint8_t rel_cursor_pos);
void Draw_Cursor(uint8_t line_count, uint8_t cursor_pos);

// Các hàm tiện ích hiển thị
void Show_Message(const char* message, uint16_t delay_ms_time);
void Show_LED_Message(uint8_t led_idx, const char* action, uint16_t delay_ms_time);
void OLED_ClearLine(uint8_t line);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width);

// Các hàm điều hướng
void Handle_LED_Navigation(uint8_t *led_cursor_pos, uint8_t *led_page);
void Handle_Calibration_Navigation(uint8_t *calib_cursor_pos, uint8_t max_pos);

// Cập nhật hiển thị LED
void Update_Led(uint8_t show_calibrated, uint8_t led_cursor_pos);

// Lấy giá trị cảm biến
void get_value(void);

#endif