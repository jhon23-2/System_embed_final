/* 
 * File: lcd_i2c.c
 * LCD I2C Library Implementation for PIC16F887
 */

#include <xc.h>
#include <stdint.h>
#include "i2c.h"
#include "lcd_i2c.h"

#define _XTAL_FREQ 20000000

void Lcd_Init(void)
{
    __delay_ms(20);
    Lcd_Cmd(0x33);
    Lcd_Cmd(0x32);
    Lcd_Cmd(0x28);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x06);
    Lcd_Cmd(0x01);
    __delay_ms(3);
}

void Lcd_Cmd(unsigned char cmd)
{
    char data_u, data_l;
    data_u = (cmd & 0xF0);
    data_l = ((cmd<<4) & 0xF0); 
   
    
    I2C_Start();
    I2C_Write(ADDRESS_LCD);
    I2C_Write(data_u|0x0C);
    I2C_Write(data_u|0x08);
    I2C_Write(data_l|0x0C);
    I2C_Write(data_l|0x08);
    I2C_Stop();
}

void Lcd_Write_Char(char c)
{
    char data_u, data_l;
    data_u = (c & 0xF0);
    data_l = ((c<<4) & 0xF0);
    
    I2C_Start();
    I2C_Write(ADDRESS_LCD);
    I2C_Write(data_u|0x0D);
    I2C_Write(data_u|0x09);
    I2C_Write(data_l|0x0D);
    I2C_Write(data_l|0x09);
    I2C_Stop();
}

void Lcd_Set_Cursor(char col, char row)
{
    unsigned char address;
    switch(row)
    {
        case 1:
            address = 0x00;
            break;
        case 2:
            address = 0x40;
            break;
        case 3:
            address = 0x14;
            break;
        case 4:
            address = 0x54;
            break;
        default:
            address = 0x00;
            break;
    }
    address += col - 1;
    Lcd_Cmd(0x80 | address);
}

void Lcd_Write_String(const char *str)
{
    while(*str != '\0')
    {
        Lcd_Write_Char(*str++);
    }
}

void Lcd_Clear(void)
{
    Lcd_Cmd(0x01);
    __delay_ms(2);
}

void Lcd_Shift_Right(void)
{
    Lcd_Cmd(0x1C);
}

void Lcd_Shift_Left(void)
{
    Lcd_Cmd(0x18);
}

void Lcd_Blink(void)
{
    Lcd_Cmd(0x0F);
}

void Lcd_NoBlink(void)
{
    Lcd_Cmd(0x0C);
}

void Lcd_CGRAM_WriteChar(char n)
{
    Lcd_Write_Char(n);
}

void Lcd_CGRAM_CreateChar(char pos, const char* new_char)
{
    if(pos < 8)
    {
        Lcd_Cmd(0x40 + (pos*8));
        for(char i=0; i<8; i++)
        {
            Lcd_Write_Char(new_char[i]);
        }
    }
}