#ifndef __I2C__
#define __I2C__

#include "define.h"

#define IIC_SCL_PORT    GPIOB
#define IIC_SCL_PINS    GPIO_Pin_8

#define IIC_SDA_PORT    GPIOB
#define IIC_SDA_PINS    GPIO_Pin_9

#define IIC_NO_ACK		1
#define IIC_ACK			0

#define OLED_SCL_Set()  GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PINS)
#define OLED_SCL_Clr()  GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PINS)

#define OLED_SDA_Set()  GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PINS)
#define OLED_SDA_Clr()  GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PINS)
#define OLED_READ_SDA() GPIO_ReadOutputDataBit(IIC_SDA_PORT, IIC_SDA_PINS)

void IIC_Start(void);
void IIC_Stop(void);
void IIC_GPIO_Init(void);
unsigned char IIC_Wait_Ack(void);
void IIC_Send_Ack(unsigned char ack);
void Write_IIC_Byte(unsigned char IIC_Byte);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);

#endif
