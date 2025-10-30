/*============================================================================
 * Licencia:
 * Autor:       Nahuel Espinosa (Adaptado para usar con i2c.h/i2c.c)
 * Fecha:       20/10/2019
 *===========================================================================*/
/** @file
 * @brief    Contiene las definiciones para el módulo DS1307 RTC
 * 
 * Implementación adaptada para trabajar con la librería I2C existente
 */

/*==================[inclusiones]============================================*/
#include <xc.h>
#include "ds1307.h"
#include "i2c.h"

/*==================[definiciones y macros]==================================*/
// Direcciones de registros del DS1307
#define RTC_SECONDS_ADDR     (0x00)
#define RTC_MINUTES_ADDR     (0x01)
#define RTC_HOURS_ADDR       (0x02)
#define RTC_DAY_ADDR         (0x03)
#define RTC_DATE_ADDR        (0x04)
#define RTC_MONTH_ADDR       (0x05)
#define RTC_YEAR_ADDR        (0x06)
#define RTC_CONTROL_ADDR     (0x07)

// Configuración del DS1307
#define RTC_OUTPUT_OFF       (0x00)
#define RTC_SQW_DISABLED     (0x00)

// Dirección I2C del DS1307 (0x68 << 1 = 0xD0 para escritura, 0xD1 para lectura)
#define RTC_I2C_ADDR_WRITE   (0xD0)
#define RTC_I2C_ADDR_READ    (0xD1)

/*==================[definiciones de funciones internas]=====================*/

/**
 * @brief Convierte un número BCD a entero decimal
 * @param bcd_byte Byte en formato BCD
 * @return Valor en decimal
 */
static uint8_t BCD_To_Dec(uint8_t bcd_byte) {
    uint8_t decenas = (bcd_byte & 0xF0) >> 4;
    uint8_t unidades = (bcd_byte & 0x0F);
    return (decenas * 10) + unidades;
}

/**
 * @brief Convierte un número decimal a BCD
 * @param dec_byte Byte en formato decimal
 * @return Valor en BCD
 */
static uint8_t Dec_To_BCD(uint8_t dec_byte) {
    uint8_t decenas = dec_byte / 10;
    uint8_t unidades = dec_byte % 10;
    return (decenas << 4) | unidades;
}

/*==================[definiciones de funciones externas]=====================*/

/**
 * @brief Configura el DS1307
 * @note Debe llamarse DESPUÉS de I2C_Init_Master()
 */
void DS1307_Init(void) {
    // Configurar registro de control: salida deshabilitada, SQW off
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_CONTROL_ADDR);
    I2C_Write(RTC_OUTPUT_OFF | RTC_SQW_DISABLED);
    I2C_Stop();
    
    // Habilitar el reloj (bit 7 del registro de segundos debe ser 0)
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_SECONDS_ADDR);
    I2C_Write(0x00);  // Segundos = 0 y CH (Clock Halt) = 0
    I2C_Stop();
}

/**
 * @brief Establece la fecha en el RTC
 */
void DS1307_SetDate(uint8_t day, uint8_t month, uint8_t year) {
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_DATE_ADDR);  // Apuntar al registro de fecha
    
    I2C_Write(Dec_To_BCD(day));
    I2C_Write(Dec_To_BCD(month));
    I2C_Write(Dec_To_BCD(year));
    
    I2C_Stop();
}

/**
 * @brief Establece la hora en el RTC
 */
void DS1307_SetTime(uint8_t hour, uint8_t min, uint8_t sec) {
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_SECONDS_ADDR);  // Apuntar al registro de segundos
    
    I2C_Write(Dec_To_BCD(sec));
    I2C_Write(Dec_To_BCD(min));
    I2C_Write(Dec_To_BCD(hour));  // Formato 24 horas
    
    I2C_Stop();
}

/**
 * @brief Obtiene la fecha del RTC
 */
void DS1307_GetDate(uint8_t *day, uint8_t *month, uint8_t *year) {
    // Establecer puntero al registro de fecha
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_DATE_ADDR);
    
    // Leer fecha
    I2C_Restart();
    I2C_Write(RTC_I2C_ADDR_READ);
    
    *day = BCD_To_Dec(I2C_Read());
    I2C_Ack();
    
    *month = BCD_To_Dec(I2C_Read());
    I2C_Ack();
    
    *year = BCD_To_Dec(I2C_Read());
    I2C_Nack();  // Último byte, enviar NACK
    
    I2C_Stop();
}

/**
 * @brief Obtiene la hora del RTC
 */
void DS1307_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec) {
    uint8_t temp;
    
    // Establecer puntero al registro de segundos
    I2C_Start();
    I2C_Write(RTC_I2C_ADDR_WRITE);
    I2C_Write(RTC_SECONDS_ADDR);
    
    // Leer hora
    I2C_Restart();
    I2C_Write(RTC_I2C_ADDR_READ);
    
    temp = I2C_Read();
    *sec = BCD_To_Dec(temp & 0x7F);  // Ignorar bit CH (Clock Halt)
    I2C_Ack();
    
    *min = BCD_To_Dec(I2C_Read());
    I2C_Ack();
    
    temp = I2C_Read();
    *hour = BCD_To_Dec(temp & 0x3F);  // Ignorar bit de formato 12/24h
    I2C_Nack();  // Último byte, enviar NACK
    
    I2C_Stop();
}

/*==================[fin del archivo]========================================*/