#ifndef _bsp_OLED_h_
#define _bsp_OLED_h_
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>


void OLED_EN(void);
void I2c_Write_byte(uint8_t byte, uint8_t mode);
void oled_clean(void);
void OLED_SetCursor(uint8_t y, uint8_t x);
void oled_Write_Char(int page, int x, char ch);
void oled_Write_String(int X, int Y, char *String);
void oled_Write_Num(int page, int x, uint16_t num, uint16_t length);
void refresh(void);

#endif

