#include "oled.h"
#include "font.h"

void OLED_WR_Byte(unsigned char dat, unsigned char cmd)
{
  if(cmd)
    Write_IIC_Data(dat);
  else
    Write_IIC_Command(dat);
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
  OLED_WR_Byte(0xB0 + y, OLED_CMD);           // Page address (0–7)
  OLED_WR_Byte((x & 0x0F), OLED_CMD);         // Lower column address
  OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); // Higher column address
}

void OLED_Display_On(void)
{
  OLED_WR_Byte(0x8D, OLED_CMD);  // Charge Pump
  OLED_WR_Byte(0x14, OLED_CMD);
  OLED_WR_Byte(0xAF, OLED_CMD);  // Display ON
}

void OLED_Display_Off(void)
{
  OLED_WR_Byte(0x8D, OLED_CMD);  // Charge Pump
  OLED_WR_Byte(0x10, OLED_CMD);
  OLED_WR_Byte(0xAE, OLED_CMD);  // Display OFF
}

void OLED_Clear(void)
{
  for (uint8_t i = 0; i < 8; i++) {
    OLED_WR_Byte(0xB0 + i, OLED_CMD);  // Page address
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    for (uint8_t j = 0; j < 128; j++) {
      OLED_WR_Byte(0x00, OLED_DATA);
    }
  }
}

void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr)
{
  unsigned char c = chr - ' ';
  if (x > Max_Column - 1) { x = 0; y += 2; }

  if (SIZE == 16) {
    OLED_Set_Pos(x, y);
    for (uint8_t i = 0; i < 8; i++)
      OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);

    OLED_Set_Pos(x, y + 1);
    for (uint8_t i = 0; i < 8; i++)
      OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
  } else {
    OLED_Set_Pos(x, y);
    for (uint8_t i = 0; i < 6; i++)
      OLED_WR_Byte(F6x8[c][i], OLED_DATA);
  }
}

unsigned int oled_pow(unsigned char m, unsigned char n)
{
  unsigned int result = 1;
  while (n--) result *= m;
  return result;
}

void OLED_ShowNum(unsigned char x, unsigned char y, uint16_t num)
{
  char number[10];
  uint8_t length = 0;

  if (num == 0) {
    OLED_ShowChar(x, y, '0');
    return;
  }

  while (num) {
    number[length++] = num % 10;
    num /= 10;
  }

  for (int i = length - 1; i >= 0; i--) {
    OLED_ShowChar(x, y, '0' + number[i]);
    x += 8;
  }
}

void OLED_ShowFloat(unsigned char x, unsigned char y, float num)
{
  if (num < 0) {
    OLED_ShowChar(x, y, '-');
    x += 8;
    num = -num;
  }

  int int_part = (int)num;
  int dec_part = (int)((num - int_part) * 100 + 0.5);

  OLED_ShowNum(x, y, int_part);
  while (int_part >= 10) {
    x += 8;
    int_part /= 10;
  }
  x += 8;

  OLED_ShowChar(x, y, '.');
  x += 8;

  OLED_ShowChar(x, y, '0' + (dec_part / 10));
  x += 8;
  OLED_ShowChar(x, y, '0' + (dec_part % 10));
}

void OLED_ShowString(unsigned char x, unsigned char y, char *chr)
{
  unsigned char j = 0;
  while (chr[j] != '\0') {
    OLED_ShowChar(x, y, chr[j]);
    x += 8;
    if (x > 120) {
      x = 0;
      y += (SIZE == 16 ? 2 : 1); // chỉ nhảy xuống 1 dòng phù hợp
    }
    j++;
  }
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
  unsigned int j = 0;
  unsigned char y = 0;

  if (y1 % 8 == 0) y = y1 / 8;
  else y = y1 / 8 + 1;

  for (unsigned char i = y0; i < y1; i++) {
    OLED_Set_Pos(x0, i);
    for (unsigned char x = x0; x < x1; x++) {
      OLED_WR_Byte(BMP[j++], OLED_DATA);
    }
  }
}

void OLED_Init(void)
{
  IIC_GPIO_Init();
  delay_ms(100);

  OLED_WR_Byte(0xAE, OLED_CMD); // Display OFF

  OLED_WR_Byte(0x20, OLED_CMD); // Memory addressing mode
  OLED_WR_Byte(0x10, OLED_CMD); // Page addressing mode

  OLED_WR_Byte(0xB0, OLED_CMD); // Page start address
  OLED_WR_Byte(0xC8, OLED_CMD); // COM output scan direction: remapped mode

  OLED_WR_Byte(0x00, OLED_CMD); // Lower column start
  OLED_WR_Byte(0x10, OLED_CMD); // Higher column start

  OLED_WR_Byte(0x40, OLED_CMD); // Display start line
  OLED_WR_Byte(0x81, OLED_CMD); // Contrast
  OLED_WR_Byte(0xFF, OLED_CMD);

  OLED_WR_Byte(0xA1, OLED_CMD); // Segment re-map
  OLED_WR_Byte(0xA6, OLED_CMD); // Normal display

  OLED_WR_Byte(0xA8, OLED_CMD);
  OLED_WR_Byte(0x3F, OLED_CMD); // Multiplex ratio for 64 rows ✅ FIXED

  OLED_WR_Byte(0xD3, OLED_CMD);
  OLED_WR_Byte(0x00, OLED_CMD); // Display offset

  OLED_WR_Byte(0xD5, OLED_CMD);
  OLED_WR_Byte(0x80, OLED_CMD); // Clock divide ratio

  OLED_WR_Byte(0xD9, OLED_CMD);
  OLED_WR_Byte(0xF1, OLED_CMD); // Pre-charge period

  OLED_WR_Byte(0xDA, OLED_CMD);
  OLED_WR_Byte(0x12, OLED_CMD); // COM pins config ✅ fixed

  OLED_WR_Byte(0xDB, OLED_CMD);
  OLED_WR_Byte(0x40, OLED_CMD); // VCOMH

  OLED_WR_Byte(0x8D, OLED_CMD);
  OLED_WR_Byte(0x14, OLED_CMD); // Charge pump enable

  OLED_WR_Byte(0xAF, OLED_CMD); // Display ON
  OLED_Clear();
  OLED_Set_Pos(0, 0);
}
