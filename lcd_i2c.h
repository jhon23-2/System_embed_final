/* 
 * File: lcd_i2c.h
 * LCD I2C Library Header for PIC16F887
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#define ADDRESS_LCD 0x4E  //if this now works, you must use any of them -> 0x7E o 0x50

void Lcd_Init(void);
void Lcd_Cmd(unsigned char cmd);
void Lcd_Set_Cursor(char col, char row);
void Lcd_Write_Char(char c);
void Lcd_Write_String(const char *str);
void Lcd_Clear(void);
void Lcd_Shift_Right(void);
void Lcd_Shift_Left(void);
void Lcd_Blink(void);
void Lcd_NoBlink(void);
void Lcd_CGRAM_WriteChar(char n);
void Lcd_CGRAM_CreateChar(char pos, const char* new_char);

#endif /* LCD_I2C_H */