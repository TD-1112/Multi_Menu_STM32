#include "define.h"

void Init(void);
void Run(void);

int main(void)
{
    // Các khởi tạo hệ thống...
    Init();
    
    // Đảm bảo biến calibrated_angle được khởi tạo đúng
    float calibrated_angle = 0;
    
    while(1)
    {
        // Đọc giá trị góc MPU
        float raw_angle = 80;
        
        // Đọc các giá trị LED cần thiết
        uint16_t value_1 = 0; // Lấy giá trị LED1
        uint16_t value_2 = 0; // Lấy giá trị LED2
        uint16_t value_3 = 0; // Lấy giá trị LED3
        uint16_t value_4 = 0; // Lấy giá trị LED4
        uint16_t value_5 = 0; // Lấy giá trị LED5
        uint16_t value_6 = 0; // Lấy giá trị LED6
        
        // Gọi Main_Menu
        Main_Menu(value_1, value_2, value_3, value_4, value_5, value_6, raw_angle);
    }
}

void Init(void)
{    
    Delay_Init();
    Button_Init();
    //MPU6050.Init();
    USART1_Init(9600);
    Led_Debug();
    OLED_Init();  
    OLED_Clear(); 
}

