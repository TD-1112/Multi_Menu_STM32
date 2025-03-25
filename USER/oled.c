#include "oled.h"
#include "font.h"

void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
  if(cmd) 
    {
      Write_IIC_Data(dat); 
    }
  else {
      Write_IIC_Command(dat); 
    }
}


void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
  OLED_WR_Byte(0xb0+y,OLED_CMD);	
  OLED_WR_Byte((x&0x0f),OLED_CMD);	
  OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	
}   	  


void OLED_Display_On(void)
{
  OLED_WR_Byte(0X8D,OLED_CMD);  
  OLED_WR_Byte(0X14,OLED_CMD);  
  OLED_WR_Byte(0XAF,OLED_CMD); 
}

void OLED_Display_Off(void)
{
  OLED_WR_Byte(0X8D,OLED_CMD);  
  OLED_WR_Byte(0X10,OLED_CMD);  
  OLED_WR_Byte(0XAE,OLED_CMD);  
}		   			 

void OLED_Clear(void)  
{  
  unsigned char i,n;		  
  for(i=0;i<8;i++)  
    {  
      OLED_WR_Byte (0xb0+i,OLED_CMD);   
      OLED_WR_Byte (0x00,OLED_CMD);     
      OLED_WR_Byte (0x10,OLED_CMD);      
      for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
    }
}

void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
  unsigned char c=0,i=0;	
  c=chr-' '; 
  if(x>Max_Column-1){x=0;y=y+2;}
  if(SIZE ==16) 
    {
      OLED_Set_Pos(x,y);	
      for(i=0;i<8;i++)  
        OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); 
      OLED_Set_Pos(x,y+1); 
      for(i=0;i<8;i++) 
        OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA); 
    }
  else  
    {	
      OLED_Set_Pos(x,y); 
      for(i=0;i<6;i++)
      OLED_WR_Byte(F6x8[c][i],OLED_DATA);
    }
}

unsigned int oled_pow(unsigned char m,unsigned char n)
{
  unsigned int result=1;	 
  while(n--)result*=m;    
  return result;
}

void OLED_ShowNum(unsigned char x, unsigned char y, uint16_t num)
{
  unsigned char x_pos = x;
  if(num == 0) 
  {
    OLED_ShowChar(x_pos, y, '0');
    return;
  }
  char number[10];  // Increased size to handle larger numbers
  uint8_t length = 0;
  
  while(num)
  {
    number[length] = num % 10;
    length++;
    num = num / 10;
  }

  for(int i = length - 1; i >= 0; i--)
  {
    OLED_ShowChar(x_pos, y, '0' + number[i]); 
    x_pos += 8;
  }
} 


void OLED_ShowFloat(unsigned char x, unsigned char y, float num)
{
  unsigned char x_pos = x;
  
  // Xử lý số âm
  if(num < 0) {
    OLED_ShowChar(x_pos, y, '-');
    x_pos += 8;
    num = -num;
  }
  
  // Lấy phần nguyên
  int int_part = (int)num;
  
  // Hiển thị phần nguyên
  if(int_part == 0) {
    OLED_ShowChar(x_pos, y, '0');
    x_pos += 8;
  } else {
    // Chuyển phần nguyên thành chuỗi số
    char int_digits[10];
    uint8_t length = 0;
    
    int temp = int_part;
    while(temp > 0) {
      int_digits[length++] = temp % 10;
      temp /= 10;
    }
    
    // Hiển thị từng chữ số của phần nguyên
    for(int i = length - 1; i >= 0; i--) {
      OLED_ShowChar(x_pos, y, '0' + int_digits[i]);
      x_pos += 8;
    }
  }
  
  // Hiển thị dấu thập phân
  OLED_ShowChar(x_pos, y, '.');
  x_pos += 8;
  
  // Lấy và hiển thị 2 chữ số sau dấu thập phân
  int decimal_part = (int)((num - int_part) * 100 + 0.5); // +0.5 để làm tròn
  
  // Đảm bảo hiển thị đủ 2 chữ số thập phân, kể cả khi là 0
  if(decimal_part < 10) {
    OLED_ShowChar(x_pos, y, '0'); // Thêm số 0 phía trước
    x_pos += 8;
    OLED_ShowChar(x_pos, y, '0' + decimal_part);
  } else {
    OLED_ShowChar(x_pos, y, '0' + (decimal_part / 10));
    x_pos += 8;
    OLED_ShowChar(x_pos, y, '0' + (decimal_part % 10));
  }
}

void OLED_ShowString(unsigned char x,unsigned char y,char *chr)
{
  unsigned char j=0; 
  while (chr[j]!='\0') 
    {		
      OLED_ShowChar(x,y,chr[j]); 
      x+=8; 
      if(x>120){x=0;y+=2;} 
        j++;
    }
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0; 
 unsigned char x,y; 
  
  if(y1%8==0) y=y1/8;   
  else y=y1/8+1;
  
  for(y=y0;y<y1;y++) 
    {
      OLED_Set_Pos(x0,y);
      for(x=x0;x<x1;x++) 
        {
          OLED_WR_Byte(BMP[j++],OLED_DATA);	  	
        }
    }
} 
   
void OLED_Init(void)
{
  IIC_GPIO_Init();				
  delay_ms(100);        

  OLED_WR_Byte(0xAE,OLED_CMD);	
  OLED_WR_Byte(0x2e,OLED_CMD);	

  OLED_WR_Byte(0x00,OLED_CMD);	
  OLED_WR_Byte(0x10,OLED_CMD);	
  OLED_WR_Byte(0x40,OLED_CMD);	
  OLED_WR_Byte(0xB0,OLED_CMD);	

  OLED_WR_Byte(0x81,OLED_CMD); 	
  OLED_WR_Byte(0xFF,OLED_CMD);	

  OLED_WR_Byte(0xA1,OLED_CMD);	
  OLED_WR_Byte(0xA6,OLED_CMD);	

  OLED_WR_Byte(0xA8,OLED_CMD);	
  OLED_WR_Byte(0x1F,OLED_CMD);	
          
  OLED_WR_Byte(0xC8,OLED_CMD);	

  OLED_WR_Byte(0xD3,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);	
          
  OLED_WR_Byte(0xD5,OLED_CMD);	
  OLED_WR_Byte(0x80,OLED_CMD);	
          
  OLED_WR_Byte(0xD9,OLED_CMD);	
  OLED_WR_Byte(0x1F,OLED_CMD);
          
  OLED_WR_Byte(0xDA,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);
          
  OLED_WR_Byte(0xDB,OLED_CMD);	
  OLED_WR_Byte(0x30,OLED_CMD);
          
  OLED_WR_Byte(0x8D,OLED_CMD);	
  OLED_WR_Byte(0x14,OLED_CMD);	

  OLED_WR_Byte(0xA4,OLED_CMD);
  OLED_WR_Byte(0xA6,OLED_CMD);

  OLED_WR_Byte(0xAF,OLED_CMD);
  OLED_Clear();        
  OLED_Set_Pos(0,0); 	 
}  

void OLED_Scroll(void)
{
  OLED_WR_Byte(0x2E,OLED_CMD);	
  OLED_WR_Byte(0x27,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);	
  OLED_WR_Byte(0x01,OLED_CMD);	
  OLED_WR_Byte(0x00,OLED_CMD);	
  OLED_WR_Byte(0xFF,OLED_CMD);	
  OLED_WR_Byte(0x2F,OLED_CMD);	
}



