/*============================================================================
 * Sistema con Pantallas Alternadas
 * Pantalla 1 (5 seg): Hora y Fecha
 * Pantalla 2 (5 seg): Temperatura y Humedad
 * PIC16F887 @ 20MHz
 *===========================================================================*/
#include <xc.h>
#include <stdio.h>
#include "i2c.h"
#include "ds1307.h"
#include "lcd_i2c.h"
#include "dht11.h"

// Bits de configuración
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
    float temp_f, hum_f;
    uint8_t hora, min, seg;
    uint8_t dia, mes, ano;
    char buffer[17];
    uint8_t dht_ok = 0;
    
    uint8_t pantalla_actual = 0;  
    uint8_t contador_segundos = 0;
    uint8_t contador_dht = 0;
    
    
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    __delay_ms(500);
    
  
    I2C_Init_Master(I2C_100KHZ);
    __delay_ms(100);
    
    Lcd_Init();
    __delay_ms(100);
    
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("  Estacion");
    Lcd_Set_Cursor(1, 2);
    Lcd_Write_String(" Meteorologica");
    __delay_ms(2000);
    
    dht11_config();
    __delay_ms(100);
    
    ds1307_init();
    __delay_ms(100);
    
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String(" Configurando");
    Lcd_Set_Cursor(1, 2);
    Lcd_Write_String(" sistema...");
    
   
    ds1307_set_time(19, 30, 0);     // 19:30:00
    ds1307_set_date(29, 10, 25);    // 29/10/2025 
    
    __delay_ms(1500);
    
    dht_ok = dht11_read(&hum_f, &temp_f);
    
    Lcd_Clear();
    
    while(1)
    {
        ds1307_get_time(&hora, &min, &seg);
        ds1307_get_date(&dia, &mes, &ano);
        
        if(contador_dht >= 3) {
            dht_ok = dht11_read(&hum_f, &temp_f);
            contador_dht = 0;
        }
        contador_dht++;
        
        if(contador_segundos >= 5) {
            pantalla_actual = !pantalla_actual;  
            contador_segundos = 0;
            Lcd_Clear();  
            __delay_ms(2);
        }
        
        if(pantalla_actual == 0) {
            Lcd_Set_Cursor(1, 1);
            sprintf(buffer, "   %02d:%02d:%02d", hora, min, seg);
            Lcd_Write_String(buffer);
            
            Lcd_Set_Cursor(1, 2);
            sprintf(buffer, " %02d/%02d/20%02d", dia, mes, ano);
            Lcd_Write_String(buffer);
            
        } else {
            Lcd_Set_Cursor(1, 1);
            if(dht_ok) {
                int temp_ent = (int)temp_f;
                int temp_dec = (int)((temp_f - temp_ent) * 10);
                sprintf(buffer, "Temp: %d.%d C", temp_ent, temp_dec);
            } else {
                sprintf(buffer, "Temp: -- C");
            }
            Lcd_Write_String(buffer);
            
            Lcd_Set_Cursor(1, 2);
            if(dht_ok) {
                int hum_ent = (int)hum_f;
                int hum_dec = (int)((hum_f - hum_ent) * 10);
                sprintf(buffer, "Hum : %d.%d %%", hum_ent, hum_dec);
            } else {
                sprintf(buffer, "Hum : -- %%");
            }
            Lcd_Write_String(buffer);
        }
        
        contador_segundos++;
        __delay_ms(1000); 
    }
}