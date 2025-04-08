/*
 * @file       I2C library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/



#include "i2c_2.h"
#include "delay.h"
#include "uart.h"

/** Defines ----------------------------------------------------------------- */
#define I2Cx_RCC				RCC_APB1Periph_I2C2
#define I2Cx						I2C2
#define I2C_GPIO_RCC		RCC_APB2Periph_GPIOB
#define I2C_GPIO				GPIOB
#define I2C_PIN_SDA			GPIO_Pin_11
#define I2C_PIN_SCL			GPIO_Pin_10

#define TIMEOUT  20000


static uint8_t numOfBytesToReceive = 0;

I2C_TypedefStruct I2C;

/** Private function prototypes --------------------------------------------- */
uint8_t i2c_start(void);
void i2c_stop(void);
uint8_t i2c_address_direction(uint8_t address, uint8_t direction);
uint8_t i2c_transmit(uint8_t byte);
uint8_t i2c_receive_ack(void);
uint8_t i2c_receive_nack(void);

void i2c_init(void);
uint8_t i2c_write_with_reg(uint8_t address, uint8_t reg, uint8_t data);
uint8_t i2c_read_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len);

void i2c_write_no_reg(uint8_t address, uint8_t data);
void i2c_write_multi_no_reg(uint8_t address, uint8_t* data, uint8_t len);
void i2c_write_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len);
void i2c_read_no_reg(uint8_t address, uint8_t* data);
void i2c_read_with_reg(uint8_t address, uint8_t reg, uint8_t* data);
void i2c_read_multi_no_reg(uint8_t address, uint8_t len, uint8_t* data);
uint8_t I2C_Reset_Bus(void);

void I2C_Start(void)__attribute__((constructor));
void I2C_Start(void)
{
    I2C.Init = i2c_init;
    I2C.Write = i2c_write_with_reg;
    I2C.Read = i2c_read_multi_with_reg;
}

/**
  ******************************************************************************
  * @file		i2c.c
  * @author	Yohanes Erwin Setiawan
  * @date		9 March 2016
  ******************************************************************************
  */

/** Includes ---------------------------------------------------------------- */	
#include "i2c_2.h"

/** Public functions -------------------------------------------------------- */
/**
  ******************************************************************************
  *	@brief	Initialize I2C in master mode
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_init()
{
	// Initialization struct
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Step 1: Initialize I2C
	RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);
	
	// Step 2: Initialize GPIO as open drain alternate function
	RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
	
}


/**
  ******************************************************************************
  *	@brief	Write byte to slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Data byte
  * @retval	None
  ******************************************************************************
  */
void i2c_write_no_reg(uint8_t address, uint8_t data)
{
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Transmitter);
	i2c_transmit(data);
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Write byte to slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Data byte
  * @retval	None
  ******************************************************************************
  */
uint8_t i2c_write_with_reg(uint8_t address, uint8_t reg, uint8_t data)
{
    // Khởi tạo I2C và kiểm tra nếu đang bận
    if (!i2c_start()) {
        i2c_stop();
        return 0;
    }
    
    // Gửi địa chỉ và kiểm tra phản hồi
    if (!i2c_address_direction(address << 1, I2C_Direction_Transmitter)) {
        i2c_stop();
        return 0;
    }
    
    // Gửi địa chỉ thanh ghi và kiểm tra phản hồi
    if (!i2c_transmit(reg)) {
        i2c_stop();
        return 0;
    }
    
    // Gửi dữ liệu và kiểm tra phản hồi
    if (!i2c_transmit(data)) {
        i2c_stop();
        return 0;
    }
    
    // Kết thúc truyền
    i2c_stop();
    
    return 1; // Trả về thành công
}

/**
  ******************************************************************************
  *	@brief	Write bytes to slave without specify register address where to
  *					start write
  * @param	Slave device address (7-bit right aligned)
  * @param	Pointer to data byte array
  * @param	Number of bytes to write
  * @retval	None
  ******************************************************************************
  */
void i2c_write_multi_no_reg(uint8_t address, uint8_t* data, uint8_t len)
{
	int i;
	
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Transmitter);
	for (i = 0; i < len; i++)
	{
		i2c_transmit(data[i]);
	}
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Write bytes to slave with specify register address where to
  *					start write
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address where to start write
  * @param	Pointer to data byte array
  * @param	Number of bytes to write
  * @retval	None
  ******************************************************************************
  */
void i2c_write_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len)
{
	int i;
	
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Transmitter);
	i2c_transmit(reg);
	for (i = 0; i < len; i++)
	{
		i2c_transmit(data[i]);
	}
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Read byte from slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Pointer to data byte to store data from slave
  * @retval	None
  ******************************************************************************
  */
void i2c_read_no_reg(uint8_t address, uint8_t* data)
{
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack();
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Read byte from slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Pointer to data byte to store data from slave
  * @retval	None
  ******************************************************************************
  */
void i2c_read_with_reg(uint8_t address, uint8_t reg, uint8_t* data)
{
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Transmitter);
	//i2c_transmit(reg);
	i2c_stop();
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack();
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Read bytes from slave without specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Number of data bytes to read from slave 
  * @param	Pointer to data array byte to store data from slave
  * @retval	None
  ******************************************************************************
  */
void i2c_read_multi_no_reg(uint8_t address, uint8_t len, uint8_t* data)
{
	int i;
	
	i2c_start();
	i2c_address_direction(address << 1, I2C_Direction_Receiver);
	for (i = 0; i < len; i++)
	{
		if (i == (len - 1))
		{
			data[i] = i2c_receive_nack();
		}
		else
		{
			data[i] = i2c_receive_ack();
		}
	}
	i2c_stop();
}

/**
  ******************************************************************************
  *	@brief	Read bytes from slave with specify register address
  * @param	Slave device address (7-bit right aligned)
  * @param	Register address
  * @param	Number of data bytes to read from slave 
  * @param	Pointer to data array byte to store data from slave
  * @retval	None
  ******************************************************************************
  */
uint8_t i2c_read_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len)
{
    int i;
    
    if (!i2c_start()) {
        i2c_stop();
        return 0;
    }
    
    if (!i2c_address_direction(address << 1, I2C_Direction_Transmitter)) {
        i2c_stop();
        return 0;
    }
    
    if (!i2c_transmit(reg)) {
        i2c_stop();
        return 0;
    }
    
    i2c_stop();
    
    if (!i2c_start()) {
        i2c_stop();
        return 0;
    }
    
    if (!i2c_address_direction(address << 1, I2C_Direction_Receiver)) {
        i2c_stop();
        return 0;
    }
    
    for (i = 0; i < len; i++)
    {
        if (i == (len - 1))
        {
            data[i] = i2c_receive_nack();
        }
        else
        {
            data[i] = i2c_receive_ack();
        }
    }
    
    i2c_stop();
    
    return 1; // Trả về thành công
}

/**
  ******************************************************************************
  * @brief Reset I2C bus using STM32 library
  * @param None
  * @retval 1
  ******************************************************************************
  */
uint8_t I2C_Reset_Bus(void)
{
    // Vô hiệu hóa I2C peripheral
    I2C_Cmd(I2Cx, DISABLE);
    
    // Reset I2C peripheral bằng thư viện RCC
    RCC_APB1PeriphResetCmd(I2Cx_RCC, ENABLE);
    delay_ms(10);
    RCC_APB1PeriphResetCmd(I2Cx_RCC, DISABLE);
    
    // Cấu hình lại GPIO pins (có thể cần thiết trong một số trường hợp kẹt bus)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Cấu hình GPIO pins thành đầu ra open-drain để giải phóng bus
    RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
    
    // Tạo xung clock thủ công để giải phóng bus

	GPIO_SetBits(I2C_GPIO, I2C_PIN_SDA);
    delay_ms(1);
    
    for(int i = 0; i < 9; i++) {
        // Toggle SCL để tạo 9 xung
        GPIO_SetBits(I2C_GPIO, I2C_PIN_SCL);
        delay_ms(1);
        GPIO_ResetBits(I2C_GPIO, I2C_PIN_SCL);
        delay_ms(1);
    }
    
    // Tạo điều kiện STOP thủ công
    GPIO_ResetBits(I2C_GPIO, I2C_PIN_SDA);
    delay_ms(1);
    GPIO_SetBits(I2C_GPIO, I2C_PIN_SCL);
    delay_ms(1);
    GPIO_SetBits(I2C_GPIO, I2C_PIN_SDA);
    delay_ms(1);
    
    // Khởi tạo lại I2C
    i2c_init();
    
    return 1;
}

/** Private functions ------------------------------------------------------- */
/**
  ******************************************************************************
  *	@brief	Generate I2C start condition
  * @param	None
  * @retval	None
  ******************************************************************************
  */
uint8_t i2c_start(void)
{
    uint16_t timeout = TIMEOUT;
    
    // Đợi cho đến khi I2Cx không bận
    while ((I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) && (timeout--)) {
        if (timeout == 0) return 0; // Trả về thất bại nếu timeout
    }
    
    // Tạo điều kiện bắt đầu
    I2C_GenerateSTART(I2Cx, ENABLE);
    
    // Đợi sự kiện I2C EV5
    timeout = TIMEOUT;
    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--)) {
        if (timeout == 0) return 0; // Trả về thất bại nếu timeout
    }
    
    return 1; // Trả về thành công
}

/**
  ******************************************************************************
  *	@brief	Generate I2C stop condition
  * @param	None
  * @retval	None
  ******************************************************************************
  */
void i2c_stop()
{
	// Generate I2C stop condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

/**
  ******************************************************************************
  *	@brief	Write slave address to I2C bus
	* @param	Slave address
	* @param	I2C direction (transmitter or receiver)
  * @retval	None
  ******************************************************************************
  */
uint8_t i2c_address_direction(uint8_t address, uint8_t direction)
{
    uint16_t timeout = TIMEOUT;
    
    // Gửi địa chỉ slave
    I2C_Send7bitAddress(I2Cx, address, direction);
    
    // Đợi cho đến khi địa chỉ được gửi xong
    if (direction == I2C_Direction_Transmitter)
    {
        while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--)) {
            if (timeout == 0) return 0; // Trả về thất bại nếu timeout
        }
    }
    else if (direction == I2C_Direction_Receiver)
    {
        while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--)) {
            if (timeout == 0) return 0; // Trả về thất bại nếu timeout
        }
    }
    
    return 1; // Trả về thành công
}

/**
  ******************************************************************************
  *	@brief	Transmit one byte to I2C bus
  * @param	Data byte to transmit
  * @retval	None
  ******************************************************************************
  */
uint8_t i2c_transmit(uint8_t byte)
{
    uint16_t timeout = TIMEOUT;
    
    // Gửi dữ liệu
    I2C_SendData(I2Cx, byte);
    
    // Đợi cho đến khi dữ liệu được gửi xong
    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--)) {
        if (timeout == 0) return 0; // Trả về thất bại nếu timeout
    }
    
    return 1; // Trả về thành công
}

/**
  ******************************************************************************
  *	@brief	Receive data byte from I2C bus, then return ACK
  * @param	None
  * @retval	Received data byte
  ******************************************************************************
  */
uint8_t i2c_receive_ack()
{
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	uint16_t timeout = TIMEOUT;
	while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)) {
        if (timeout == 0) return 0; // Trả về thất bại nếu timeout
    }
	
	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}

/**
  ******************************************************************************
  *	@brief	Receive data byte from I2C bus, then return NACK
  * @param	None
  * @retval	Received data byte
  ******************************************************************************
  */
uint8_t i2c_receive_nack()
{
	// Disable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	i2c_stop();
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	uint16_t timeout = TIMEOUT;
	while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)) {
        if (timeout == 0) return 0; // Trả về thất bại nếu timeout
    }
	
	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}

/********************************* END OF FILE ********************************/
/******************************************************************************/