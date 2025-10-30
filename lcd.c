/*============================================================================
 * Sistema básico para LCD 16x2 - Solo Hora y Fecha
 * PIC16F887 con XC8
 *===========================================================================*/
#include <xc.h>
#include <stdio.h>
#include "i2c.h"
#include "ds1307.h"
#include "lcd_i2c.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

#define _XTAL_FREQ 20000000

void main(void)
{
    uint8_t hora, min, seg;
    uint8_t dia, mes, ano;
    char linea1[17];
    char linea2[17];
    
    // Configurar puertos como digitales
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    __delay_ms(500);
    
    // Inicializar I2C
    I2C_Init_Master(I2C_100KHZ);
    __delay_ms(100);
    
    // Inicializar LCD
    Lcd_Init();
    __delay_ms(100);
    
    // Mensaje de inicio
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("  Iniciando...  ");
    __delay_ms(1000);
    
    // Inicializar DS1307
    DS1307_Init();
    __delay_ms(100);
    
    // ========================================
    // CONFIGURAR SOLO LA PRIMERA VEZ
    // Después de programar, COMENTAR estas 2 líneas:
    DS1307_SetTime(12, 0, 0);     // Hora: 12:00:00
    DS1307_SetDate(29, 10, 24);   // Fecha: 29/10/2024
    // ========================================
    
    Lcd_Clear();
    
    // Loop principal
    while(1)
    {
        // Leer hora y fecha del DS1307
        DS1307_GetTime(&hora, &min, &seg);
        DS1307_GetDate(&dia, &mes, &ano);
        
        // LÍNEA 1: Solo Hora
        Lcd_Set_Cursor(1, 1);
        sprintf(linea1, "Hora: %02d:%02d:%02d", hora, min, seg);
        Lcd_Write_String(linea1);
        
        // LÍNEA 2: Solo Fecha
        Lcd_Set_Cursor(2, 1);
        sprintf(linea2, "Fecha:%02d/%02d/%02d", dia, mes, ano);
        Lcd_Write_String(linea2);
        
        __delay_ms(1000);  // Actualizar cada segundo
    }
}