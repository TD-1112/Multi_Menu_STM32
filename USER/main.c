#include "define.h"


int main(void)
{
    Delay_Init();
		Button_Init();
    OLED_Init();  
    OLED_Clear(); 
    while(1)
    {
    uint16_t value1 = 1234;
    uint16_t value2 = 1231;
    uint16_t value3 = 1809;
    uint16_t value4 = 1123;
    uint16_t value5 = 1355;
    uint16_t value6 = 1780;
    Main_Menu(value1, value2, value3, value4, value5, value6); 
      //Main_Menu();
    }
}

