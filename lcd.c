/*
 * File: main.c
 * DHT11 con LCD I2C
 * Microcontrolador: PIC16F887
 * Cristal: 20MHz (HS)
 */
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include "i2c.h"
#include "lcd_i2c.h"
#include "dht11.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON      // Power-up Timer habilitado
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON      // Brown-out Reset habilitado
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF

#define _XTAL_FREQ 20000000

// Buffers de caracteres para la LCD
char buffer_tem[16];
char buffer_hum[16];

void float_to_string_opt(float value, char* buffer) {
    int parte_entera;
    int parte_decimal;
    
    // Manejar valores negativos (aunque DHT11 no los da)
    if (value < 0) value = -value;
    
    parte_entera = (int)value;
    
    // Primera cifra decimal
    parte_decimal = (int)((value * 10.0) - (parte_entera * 10));
    
    sprintf(buffer, "%d.%d", parte_entera, parte_decimal);
}

int main(void) 
{
    float tem, hum;
    uint8_t intentos = 0;
    
    // Configurar puertos
    ANSEL = 0x00;   // Desactivar entradas analógicas
    ANSELH = 0x00;
    
    // Inicializar I2C y LCD
    I2C_Init_Master(I2C_100KHZ);
    __delay_ms(100);
    
    Lcd_Init();
    __delay_ms(50);
    
    // Inicializar DHT11
    dht11_config();
    

    Lcd_Set_Cursor(2,1);
    Lcd_Write_String(" Hold on...");
    __delay_ms(2000);  // DHT11 necesita ~1 segundo para estabilizarse
    
    while(1) {
        if(dht11_read(&hum, &tem)) {
            // Lectura exitosa
            intentos = 0;
            
            float_to_string_opt(tem, buffer_tem);
            float_to_string_opt(hum, buffer_hum);
            
            Lcd_Clear();
            __delay_ms(2);  // Esperar a que se complete el clear
            
            // Primera línea: Temperatura
            Lcd_Set_Cursor(1, 1);
            __delay_us(50);
            Lcd_Write_String("Temp: ");
            Lcd_Write_String(buffer_tem);
            Lcd_Write_Char(' ');
            Lcd_Write_Char('C');
            
            // Segunda línea: Humedad
            Lcd_Set_Cursor(1, 2);
            __delay_us(50);
            Lcd_Write_String("Hum : ");
            Lcd_Write_String(buffer_hum);
            Lcd_Write_Char(' ');
            Lcd_Write_Char('%');
            
        } else {
            // Error en la lectura
            intentos++;
            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(" Error DHT11");
            Lcd_Set_Cursor(2,1);
            
            if(intentos < 3) {
                Lcd_Write_String(" Reintentando..");
            } else {
                Lcd_Write_String(" Check conexion");
            }
        }
        
        // DHT11 requiere mínimo 2 segundos entre lecturas
        __delay_ms(2500);
    }
    
    return 0;
}