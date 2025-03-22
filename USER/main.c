#include "define.h"


int main(void)
{
    Timer2_Init();
		Button_Init();
    OLED_Init();  
    OLED_Clear(); 
    while(1)
    {
      check_status();
      main_menu();
    }
}

