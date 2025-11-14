/*
 * File: main.c
 * DHT11 con LCD I2C + EEPROM + LEDs indicadores
 * Microcontrolador: PIC16F887
 * Cristal: 20MHz (HS)
 * 
 * DIFERENCIAS DHT11 vs DHT22:
 * - DHT11: Temp 0-50°C (±2°C), Hum 20-80% (±5%)
 * - DHT11: Solo valores enteros (sin decimales)
 * - DHT11: Tiempo de respuesta más rápido
 */
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include "i2c.h"
#include "lcd_i2c.h"
#include "dht11.h"
// Para DHT11 usar: #include "dht11.h"  (en lugar de dht22.h)

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
#define MAX_LECTURAS 30  // Más espacio por ser datos enteros (2 bytes/lectura)
#define EEPROM_BASE_ADDR 0x00

// Estructura para guardar en EEPROM (2 bytes por lectura)
typedef struct {
    uint8_t temperatura;  // Temp entera (0-50°C)
    uint8_t humedad;      // Hum entera (20-80%)
} Lectura;

// Variables globales
char buffer_tem[16];
char buffer_hum[16];
uint8_t indice_lectura = 0;
uint8_t total_lecturas = 0;
uint16_t contador_muestras = 0;

// ========== FUNCIONES EEPROM ==========
void EEPROM_Write(uint8_t addr, uint8_t data) {
    while(WR);
    EEADR = addr;
    EEDAT = data;
    EEPGD = 0;
    WREN = 1;
    
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1;
    INTCONbits.GIE = 1;
    
    while(WR);
    WREN = 0;
}

uint8_t EEPROM_Read(uint8_t addr) {
    EEADR = addr;
    EEPGD = 0;
    RD = 1;
    return EEDAT;
}

// Guardar lectura en EEPROM (solo 2 bytes por lectura)
void guardar_lectura(uint8_t temp, uint8_t hum) {
    uint8_t base_addr = EEPROM_BASE_ADDR + (indice_lectura * 2);
    
    EEPROM_Write(base_addr, temp);
    EEPROM_Write(base_addr + 1, hum);
    
    indice_lectura++;
    if(indice_lectura >= MAX_LECTURAS) {
        indice_lectura = 0;
    }
    
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
float calcular_promedio_temp(uint8_t ultimas_n) {
    if(total_lecturas == 0) return 0.0;
    
    uint16_t suma = 0;
    uint8_t n = (ultimas_n < total_lecturas) ? ultimas_n : total_lecturas;
    
    for(uint8_t i = 0; i < n; i++) {
        Lectura lec = leer_lectura(i);
        suma += lec.temperatura;
    }
    
    return (float)suma / n;
}

float calcular_promedio_hum(uint8_t ultimas_n) {
    if(total_lecturas == 0) return 0.0;
    
    uint16_t suma = 0;
    uint8_t n = (ultimas_n < total_lecturas) ? ultimas_n : total_lecturas;
    
    for(uint8_t i = 0; i < n; i++) {
        Lectura lec = leer_lectura(i);
        suma += lec.humedad;
    }
    
    return (float)suma / n;
}

// Calcular tendencia de temperatura
float calcular_tendencia_temp() {
    if(total_lecturas < 6) return 0.0;
    
    float promedio_reciente = calcular_promedio_temp(3);
    
    // Calcular promedio de 3 lecturas anteriores
    uint16_t suma_anterior = 0;
    for(uint8_t i = 3; i < 6; i++) {
        Lectura lec = leer_lectura(i);
        suma_anterior += lec.temperatura;
    }
    float promedio_anterior = (float)suma_anterior / 3.0;
    
    return promedio_reciente - promedio_anterior;
}

// Pronóstico simple
uint8_t pronostico_temperatura() {
    if(total_lecturas < 5) {
        return (uint8_t)calcular_promedio_temp(total_lecturas);
    }
    return (uint8_t)calcular_promedio_temp(5);
}

uint8_t pronostico_humedad() {
    if(total_lecturas < 5) {
        return (uint8_t)calcular_promedio_hum(total_lecturas);
    }
    return (uint8_t)calcular_promedio_hum(5);
}

// Calcular mínimo y máximo
void calcular_min_max(uint8_t *temp_min, uint8_t *temp_max, 
                      uint8_t *hum_min, uint8_t *hum_max) {
    if(total_lecturas == 0) {
        *temp_min = *temp_max = *hum_min = *hum_max = 0;
        return;
    }
    
    Lectura primera = leer_lectura(0);
    *temp_min = *temp_max = primera.temperatura;
    *hum_min = *hum_max = primera.humedad;
    
    for(uint8_t i = 1; i < total_lecturas; i++) {
        Lectura lec = leer_lectura(i);
        
        if(lec.temperatura < *temp_min) *temp_min = lec.temperatura;
        if(lec.temperatura > *temp_max) *temp_max = lec.temperatura;
        if(lec.humedad < *hum_min) *hum_min = lec.humedad;
        if(lec.humedad > *hum_max) *hum_max = lec.humedad;
    }
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
    
    // LED de pronóstico (indica tendencia fuerte)
    if(tendencia > 2.0 || tendencia < -2.0) {
        LED_PRONOSTICO = 1;
    } else {
        LED_PRONOSTICO = 0;
    }
}

// ========== PROGRAMA PRINCIPAL ==========
int main(void) 
{
    float tem, hum;  // Cambiar a float para compatibilidad con dht11_read
    uint8_t intentos = 0;
    float tendencia = 0.0;
    uint8_t pronostico_t, pronostico_h;
    uint8_t temp_min, temp_max, hum_min, hum_max;
    uint8_t modo_display = 0;  // 0=Actual, 1=Pronóstico, 2=Estadísticas
    
    // Configurar puertos
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    // Puerto D para LEDs
    PORTD = 0x00;
    TRISD = 0x00;
    
    // Puerto B para DHT11 (si es necesario configurar el pin)
    // La librería dht11.h usa RA0, no necesitas configurar RB0
    // TRISB = 0x01;  // Comentado porque el DHT11 está en RA0
    
    // Inicializar I2C y LCD
    I2C_Init_Master(I2C_100KHZ);
    __delay_ms(100);
    
    Lcd_Init();
    __delay_ms(50);
    
    dht11_config();
    __delay_ms(100);
    
    
    // Mensaje inicial
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Sistema DHT11");
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("Iniciando...");
    __delay_ms(2000);
    
    while(1) {
        if(dht11_read(&hum, &tem)) {
            // Lectura exitosa
            intentos = 0;
            contador_muestras++;
            
            // Guardar cada 10 lecturas (~20 seg para pruebas)
            // Para proyecto real: cambiar a 1800 (1 hora con DHT11)
            if(contador_muestras >= 10) {
                guardar_lectura((uint8_t)tem, (uint8_t)hum);
                contador_muestras = 0;
                
                // Actualizar análisis
                tendencia = calcular_tendencia_temp();
                pronostico_t = pronostico_temperatura();
                pronostico_h = pronostico_humedad();
                calcular_min_max(&temp_min, &temp_max, &hum_min, &hum_max);
            }
            
            // Actualizar LEDs
            actualizar_leds((uint8_t)tem, (uint8_t)hum, tendencia);
            
            // Mostrar en LCD según modo
            Lcd_Clear();
            __delay_ms(2);
            
            switch(modo_display) {
                case 0:  // Vista actual
                    Lcd_Set_Cursor(1, 1);
                    sprintf(buffer_tem, "T:%dC  H:%d%%", (int)tem, (int)hum);
                    Lcd_Write_String(buffer_tem);
                    
                    Lcd_Set_Cursor(1, 2);
                    sprintf(buffer_tem, "Mem:%d Tend:", total_lecturas);
                    Lcd_Write_String(buffer_tem);
                    if(tendencia > 1.0) {
                        Lcd_Write_Char('^');  // Subiendo
                    } else if(tendencia < -1.0) {
                        Lcd_Write_Char('v');  // Bajando
                    } else {
                        Lcd_Write_Char('-');  // Estable
                    }
                    break;
                    
                case 1:  // Vista pronóstico
                    Lcd_Set_Cursor(1, 1);
                    Lcd_Write_String("PRONOSTICO:");
                    
                    Lcd_Set_Cursor(1, 2);
                    sprintf(buffer_tem, "T:%dC  H:%d%%", pronostico_t, pronostico_h);
                    Lcd_Write_String(buffer_tem);
                    break;
                    
                case 2:  // Vista estadísticas
                    Lcd_Set_Cursor(1, 1);
                    sprintf(buffer_tem, "T:%d-%dC", temp_min, temp_max);
                    Lcd_Write_String(buffer_tem);
                    
                    Lcd_Set_Cursor(1, 2);
                    sprintf(buffer_tem, "H:%d-%d%%", hum_min, hum_max);
                    Lcd_Write_String(buffer_tem);
                    break;
            }
            
            // Rotar modo cada 4 ciclos
            static uint8_t ciclos = 0;
            ciclos++;
            if(ciclos >= 4) {
                modo_display++;
                if(modo_display > 2) modo_display = 0;
                ciclos = 0;
            }
            
        } else {
            // Error en la lectura
            intentos++;
            
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String(" Error DHT11");
            Lcd_Set_Cursor(1,2);
            
            if(intentos < 3) {
                Lcd_Write_String(" Reintentando..");
            } else {
                Lcd_Write_String(" Check conexion");
            }
            
            PORTD = 0x00;  // Apagar LEDs
        }
        
        // DHT11 requiere mínimo 1 segundo entre lecturas
        __delay_ms(2000);
    }
    
    return 0;
}
