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
      OLED_Set_Pos(x,y+1); 
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


void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
  unsigned char t,temp; 
  unsigned char enshow=0;		
  for(t=0;t<len;t++)
    {
      temp=(num/oled_pow(10,len-t-1))%10;
      if(enshow==0&&t<(len-1)) 
        {
          if(temp==0) 
            {
              OLED_ShowChar(x+(size2/2)*t,y,' ');
              continue; 
            }else enshow=1; 
        }
      OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); 
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
  Delay_ms(100);        

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



