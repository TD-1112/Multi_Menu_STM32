#ifndef __OLED___
#define __OLED___

#include "define.h"

#define OLED_CMD  		0	
#define OLED_DATA 		1	
#define OLED_MODE 		0

#define SIZE 				8
#define Max_Column	128
#define Max_Row			64 
#define X_WIDTH 		128
#define Y_WIDTH 		64 

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr);
void OLED_ShowNum(unsigned char x,unsigned char y,uint16_t num);
void OLED_ShowString(unsigned char x,unsigned char y,char *p);	
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_Scroll(void);
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
void OLED_ShowFloat(unsigned char x,unsigned char y,float num);
unsigned int oled_pow(unsigned char m,unsigned char n);


#endif


