/*
 * File: main.c
 * DHT11 con LCD I2C + EEPROM + LEDs indicadores
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
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF

#define _XTAL_FREQ 20000000

// ========== DEFINICIONES DE LEDs ==========
#define LED_FRIO     PORTDbits.RD0  // Temp < 20°C (Azul)
#define LED_NORMAL   PORTDbits.RD1  // Temp 20-28°C (Verde)
#define LED_CALOR    PORTDbits.RD2  // Temp > 28°C (Rojo)
#define LED_SECO     PORTDbits.RD3  // Hum < 40%
#define LED_HUMEDO   PORTDbits.RD4  // Hum > 70%
#define LED_PRONOSTICO PORTDbits.RD5  // Parpadea según tendencia

// ========== CONFIGURACIÓN EEPROM ==========
#define MAX_LECTURAS 30  // DHT11 permite más lecturas (2 bytes por lectura)
#define EEPROM_BASE_ADDR 0x00

// Estructura para guardar en EEPROM (2 bytes por lectura - DHT11 sin decimales)
typedef struct {
    uint8_t temperatura;  // Temperatura entera (0-50°C)
    uint8_t humedad;      // Humedad entera (20-90%)
} Lectura;

// Variables globales
char buffer_tem[16];
char buffer_hum[16];
uint8_t indice_lectura = 0;  // Índice circular para EEPROM
uint8_t total_lecturas = 0;  // Total de lecturas guardadas
uint16_t contador_muestras = 0;  // Contador para guardar cada N muestras

// ========== FUNCIONES EEPROM ==========
void EEPROM_Write(uint8_t addr, uint8_t data) {
    while(WR);  // Esperar si hay escritura en progreso
    EEADR = addr;
    EEDAT = data;
    EEPGD = 0;  // Apuntar a memoria EEPROM
    WREN = 1;   // Habilitar escritura
    
    // Secuencia obligatoria
    INTCONbits.GIE = 0;  // Deshabilitar interrupciones
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1;
    INTCONbits.GIE = 1;  // Rehabilitar interrupciones
    
    while(WR);  // Esperar fin de escritura
    WREN = 0;   // Deshabilitar escritura
}

uint8_t EEPROM_Read(uint8_t addr) {
    EEADR = addr;
    EEPGD = 0;
    RD = 1;
    return EEDAT;
}

// Guardar lectura en EEPROM (DHT11 - solo 2 bytes)
void guardar_lectura(uint8_t temp, uint8_t hum) {
    uint8_t base_addr = EEPROM_BASE_ADDR + (indice_lectura * 2);
    
    // Guardar 2 bytes directamente (sin decimales)
    EEPROM_Write(base_addr, temp);      // Temperatura
    EEPROM_Write(base_addr + 1, hum);   // Humedad
    
    // Actualizar índice circular
    indice_lectura++;
    if(indice_lectura >= MAX_LECTURAS) {
        indice_lectura = 0;
    }
    
    // Actualizar contador total
    if(total_lecturas < MAX_LECTURAS) {
        total_lecturas++;
    }
}

// Leer lectura de EEPROM
Lectura leer_lectura(uint8_t index) {
    Lectura lec;
    uint8_t base_addr = EEPROM_BASE_ADDR + (index * 2);
    
    lec.temperatura = EEPROM_Read(base_addr);
    lec.humedad = EEPROM_Read(base_addr + 1);
    
    return lec;
}

// ========== FUNCIONES DE ANÁLISIS ==========
// Calcular promedio de temperatura (últimas N lecturas)
float calcular_promedio_temp(uint8_t ultimas_n) {
    if(total_lecturas == 0) return 0.0;
    
    float suma = 0.0;
    uint8_t n = (ultimas_n < total_lecturas) ? ultimas_n : total_lecturas;
    
    for(uint8_t i = 0; i < n; i++) {
        Lectura lec = leer_lectura(i);
        suma += (float)lec.temperatura;
    }
    
    return suma / n;
}

// Calcular promedio de humedad
float calcular_promedio_hum(uint8_t ultimas_n) {
    if(total_lecturas == 0) return 0.0;
    
    float suma = 0.0;
    uint8_t n = (ultimas_n < total_lecturas) ? ultimas_n : total_lecturas;
    
    for(uint8_t i = 0; i < n; i++) {
        Lectura lec = leer_lectura(i);
        suma += (float)lec.humedad;
    }
    
    return suma / n;
}

// Calcular tendencia (positiva = calentando, negativa = enfriando)
float calcular_tendencia_temp() {
    if(total_lecturas < 6) return 0.0;
    
    // Comparar promedio de últimas 3 vs promedio de 3 anteriores
    float promedio_reciente = 0.0;
    float promedio_anterior = 0.0;
    
    for(uint8_t i = 0; i < 3; i++) {
        Lectura lec = leer_lectura(i);
        promedio_reciente += (float)lec.temperatura;
    }
    promedio_reciente /= 3.0;
    
    for(uint8_t i = 3; i < 6; i++) {
        Lectura lec = leer_lectura(i);
        promedio_anterior += (float)lec.temperatura;
    }
    promedio_anterior /= 3.0;
    
    return promedio_reciente - promedio_anterior;
}

// Pronóstico simple (promedio de últimas 5 lecturas)
float pronostico_temperatura() {
    if(total_lecturas < 5) {
        return calcular_promedio_temp(total_lecturas);
    }
    return calcular_promedio_temp(5);
}

float pronostico_humedad() {
    if(total_lecturas < 5) {
        return calcular_promedio_hum(total_lecturas);
    }
    return calcular_promedio_hum(5);
}

// ========== CONTROL DE LEDs ==========
void actualizar_leds(uint8_t temp, uint8_t hum, float tendencia) {
    // LEDs de temperatura actual
    LED_FRIO = (temp < 20) ? 1 : 0;
    LED_NORMAL = (temp >= 20 && temp <= 28) ? 1 : 0;
    LED_CALOR = (temp > 28) ? 1 : 0;
    
    // LEDs de humedad
    LED_SECO = (hum < 40) ? 1 : 0;
    LED_HUMEDO = (hum > 70) ? 1 : 0;
    
    // LED de pronóstico (indica tendencia)
    // Si la tendencia es fuerte (>1°C), el LED parpadea
    if(tendencia > 1.0 || tendencia < -1.0) {
        LED_PRONOSTICO = 1;  // Encendido = cambio significativo
    } else {
        LED_PRONOSTICO = 0;
    }
}

// ========== FUNCIÓN DE CONVERSIÓN (DHT11 - valores enteros) ==========
void int_to_string(uint8_t value, char* buffer) {
    sprintf(buffer, "%d", value);
}

// ========== PROGRAMA PRINCIPAL ==========
int main(void) 
{
    uint8_t tem, hum;  // DHT11 retorna valores enteros
    uint8_t intentos = 0;
    float tendencia = 0.0;
    float pronostico_temp = 0.0;
    float pronostico_hum = 0.0;
    bool mostrar_pronostico = false;
    
    // Configurar puertos
    ANSEL = 0x00;   // Desactivar entradas analógicas
    ANSELH = 0x00;
    
    // Configurar Puerto D para LEDs
    PORTD = 0x00;
    TRISD = 0x00;  // Todo como salida
    
    // Inicializar I2C y LCD
    I2C_Init_Master(I2C_100KHZ);
    __delay_ms(100);
    
    Lcd_Init();
    __delay_ms(50);
    
    // Inicializar DHT11
    DHT11_init();
    
    // Mensaje inicial
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Sistema DHT11");
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("Iniciando...");
    __delay_ms(2000);
    
    while(1) {
        if(DHT11_read(&hum, &tem)) {  // DHT11_read en lugar de DHT22_read
            // Lectura exitosa
            intentos = 0;
            contador_muestras++;
            
            // Guardar en EEPROM cada 10 lecturas (≈25 segundos)
            // En aplicación real sería cada hora
            if(contador_muestras >= 10) {
                guardar_lectura(tem, hum);
                contador_muestras = 0;
                
                // Calcular tendencia y pronóstico
                tendencia = calcular_tendencia_temp();
                pronostico_temp = pronostico_temperatura();
                pronostico_hum = pronostico_humedad();
            }
            
            // Convertir a string (DHT11 - sin decimales)
            int_to_string(tem, buffer_tem);
            int_to_string(hum, buffer_hum);
            
            // Actualizar LEDs
            actualizar_leds(tem, hum, tendencia);
            
            // Alternar entre vista actual y pronóstico
            Lcd_Clear();
            __delay_ms(2);
            
            if(!mostrar_pronostico) {
                // Vista actual
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("T:");
                Lcd_Write_String(buffer_tem);
                Lcd_Write_String("C H:");
                Lcd_Write_String(buffer_hum);
                Lcd_Write_Char('%');
                
                Lcd_Set_Cursor(1, 2);
                sprintf(buffer_tem, "Hist:%d Tend:", total_lecturas);
                Lcd_Write_String(buffer_tem);
                if(tendencia > 0.5) {
                    Lcd_Write_Char('^');  // Subiendo
                } else if(tendencia < -0.5) {
                    Lcd_Write_Char('v');  // Bajando
                } else {
                    Lcd_Write_Char('-');  // Estable
                }
            } else {
                // Vista pronóstico
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("PRONOSTICO:");
                
                Lcd_Set_Cursor(1, 2);
                sprintf(buffer_tem, "T:%d H:%d", (int)pronostico_temp, (int)pronostico_hum);
                Lcd_Write_String(buffer_tem);
            }
            
            // Alternar vista cada 3 ciclos
            static uint8_t ciclos = 0;
            ciclos++;
            if(ciclos >= 3) {
                mostrar_pronostico = !mostrar_pronostico;
                ciclos = 0;
            }
            
        } else {
            // Error en la lectura
            intentos++;
            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(" Error DHT11");
            Lcd_Set_Cursor(2,2);
            
            if(intentos < 3) {
                Lcd_Write_String(" Reintentando..");
            } else {
                Lcd_Write_String(" Check conexion");
            }
            
            // Apagar LEDs en caso de error
            PORTD = 0x00;
        }
        
        // DHT11 requiere mínimo 1 segundo entre lecturas (más rápido que DHT22)
        __delay_ms(2500);
    }
    
    return 0;
}