/*============================================================================
 * DS1307 RTC Library - VERSIÓN MEJORADA Y ROBUSTA
 * Compatible con PIC16F887 y XC8
 *===========================================================================*/
#include "ds1307.h"
#include "i2c.h"
#include <xc.h>

// Direcciones de registros del DS1307
#define DS1307_ADDR_WRITE   0xD0
#define DS1307_ADDR_READ    0xD1

#define DS1307_REG_SEC      0x00
#define DS1307_REG_MIN      0x01
#define DS1307_REG_HOUR     0x02
#define DS1307_REG_DAY      0x03
#define DS1307_REG_DATE     0x04
#define DS1307_REG_MONTH    0x05
#define DS1307_REG_YEAR     0x06
#define DS1307_REG_CONTROL  0x07

// Funciones de conversión BCD
static uint8_t bcd_to_dec(uint8_t val)
{
    return (uint8_t)(((val >> 4) * 10) + (val & 0x0F));
}

static uint8_t dec_to_bcd(uint8_t val)
{
    uint8_t decenas = (uint8_t)(val / 10);
    uint8_t unidades = (uint8_t)(val % 10);
    return (uint8_t)((decenas << 4) | unidades);
}

// Inicializar el DS1307
void ds1307_init(void)
{
    __delay_ms(50);
    
    // Habilitar el oscilador (limpiar bit CH en registro de segundos)
    I2C_Start();
    I2C_Write(DS1307_ADDR_WRITE);
    I2C_Write(DS1307_REG_SEC);
    I2C_Write(0x00);  // CH=0, iniciar oscilador
    I2C_Stop();
    
    __delay_ms(20);
    
    // Configurar registro de control
    I2C_Start();
    I2C_Write(DS1307_ADDR_WRITE);
    I2C_Write(DS1307_REG_CONTROL);
    I2C_Write(0x00);  // OUT=0, SQWE=0
    I2C_Stop();
    
    __delay_ms(20);
}

// Configurar la fecha
void ds1307_set_date(uint8_t day, uint8_t month, uint8_t year)
{
    I2C_Start();
    
    if(I2C_Write(DS1307_ADDR_WRITE) == 0)  // Verificar ACK
    {
        I2C_Write(DS1307_REG_DATE);
        I2C_Write(dec_to_bcd(day));
        I2C_Write(dec_to_bcd(month));
        I2C_Write(dec_to_bcd(year));
    }
    
    I2C_Stop();
    __delay_ms(20);
}

// Configurar la hora
void ds1307_set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    I2C_Start();
    
    if(I2C_Write(DS1307_ADDR_WRITE) == 0)  // Verificar ACK
    {
        I2C_Write(DS1307_REG_SEC);
        I2C_Write(dec_to_bcd(sec) & 0x7F);  // Asegurar CH=0
        I2C_Write(dec_to_bcd(min));
        I2C_Write(dec_to_bcd(hour) & 0x3F); // Formato 24h
    }
    
    I2C_Stop();
    __delay_ms(20);
}

// Leer la fecha
void ds1307_get_date(uint8_t *day, uint8_t *month, uint8_t *year)
{
    uint8_t temp;
    
    // Escribir la dirección del registro
    I2C_Start();
    
    if(I2C_Write(DS1307_ADDR_WRITE) == 0)  // Verificar ACK
    {
        I2C_Write(DS1307_REG_DATE);
        I2C_Stop();
        
        __delay_us(50);
        
        // Leer los datos
        I2C_Start();
        
        if(I2C_Write(DS1307_ADDR_READ) == 0)  // Verificar ACK
        {
            temp = I2C_Read();
            I2C_Ack();
            *day = bcd_to_dec(temp & 0x3F);
            
            temp = I2C_Read();
            I2C_Ack();
            *month = bcd_to_dec(temp & 0x1F);
            
            temp = I2C_Read();
            I2C_Nack();
            *year = bcd_to_dec(temp);
        }
        else
        {
            // Si falla, valores por defecto
            *day = 1;
            *month = 1;
            *year = 0;
        }
    }
    else
    {
        // Si falla, valores por defecto
        *day = 1;
        *month = 1;
        *year = 0;
    }
    
    I2C_Stop();
}

// Leer la hora
void ds1307_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t temp;
    
    // Escribir la dirección del registro
    I2C_Start();
    
    if(I2C_Write(DS1307_ADDR_WRITE) == 0)  // Verificar ACK
    {
        I2C_Write(DS1307_REG_SEC);
        I2C_Stop();
        
        __delay_us(50);
        
        // Leer los datos
        I2C_Start();
        
        if(I2C_Write(DS1307_ADDR_READ) == 0)  // Verificar ACK
        {
            temp = I2C_Read();
            I2C_Ack();
            *sec = bcd_to_dec(temp & 0x7F);  // Ignorar bit CH
            
            temp = I2C_Read();
            I2C_Ack();
            *min = bcd_to_dec(temp & 0x7F);
            
            temp = I2C_Read();
            I2C_Nack();
            *hour = bcd_to_dec(temp & 0x3F);  // Formato 24h
        }
        else
        {
            // Si falla, valores por defecto
            *sec = 0;
            *min = 0;
            *hour = 0;
        }
    }
    else
    {
        // Si falla, valores por defecto
        *sec = 0;
        *min = 0;
        *hour = 0;
    }
    
    I2C_Stop();
}