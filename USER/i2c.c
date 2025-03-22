#include "i2c.h"

void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    OLED_SCL_Set();
    OLED_SDA_Set();
}

void IIC_Start(void)
{

  OLED_SCL_Set();
  OLED_SDA_Set();
 // Delay_ms(1);
  OLED_SDA_Clr();
 // Delay_ms(1);
  OLED_SCL_Clr();
 // Delay_ms(1);
}



void IIC_Stop(void)
{
  OLED_SDA_Clr();
//  Delay_ms(1);	
  OLED_SCL_Set();
  //Delay_ms(1);
  OLED_SDA_Set();
 // Delay_ms(1);
}



unsigned char IIC_Wait_Ack(void)
{
  unsigned char ack;

  OLED_SCL_Clr();
  //Delay_ms(1);
  OLED_SDA_Set();
 //Delay_ms(1);
  OLED_SCL_Set();	
 //Delay_ms(1);

  if(OLED_READ_SDA())	
    ack = IIC_NO_ACK;
   else
    ack = IIC_ACK;		

  OLED_SCL_Clr();
 //Delay_ms(1);

  return ack;
}



void IIC_Send_Ack(unsigned char ack)
{
  OLED_SCL_Clr();
 // Delay_ms(1);	

  if(ack == IIC_ACK)	
    OLED_SDA_Clr();	
  else
    OLED_SDA_Set();	
// Delay_ms(1);	
  OLED_SCL_Set();	
 //Delay_ms(1);	
  OLED_SCL_Clr();	
  //Delay_ms(1);	
}



void Write_IIC_Byte(unsigned char IIC_Byte)
{
  unsigned char i;  
  for(i=0;i<8;i++)
  {
    OLED_SCL_Clr();	
   // Delay_ms(1);	

    if(IIC_Byte & 0x80)	
      OLED_SDA_Set();	
    else 
      OLED_SDA_Clr();	

    IIC_Byte <<= 1;  
   // Delay_ms(1);	
    OLED_SCL_Set(); 
   // Delay_ms(1);	
  }
  OLED_SCL_Clr();	
 // Delay_ms(1);	

  if(IIC_Wait_Ack()){}
}


void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);
   Write_IIC_Byte(0x00);
   Write_IIC_Byte(IIC_Command);
   IIC_Stop(); 

}

void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   Write_IIC_Byte(0x78);	
   Write_IIC_Byte(0x40);	
   Write_IIC_Byte(IIC_Data);
   IIC_Stop();		
}