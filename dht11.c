/*============================================================================
 * Licencia:
 * Autor:         Nahuel Espinosa
 * Fecha:         20/10/2019 (Corregido para timing correcto en PIC16F887 @ 20MHz)
 *===========================================================================*/
/** @file
 * @brief    Contiene las definiciones para el módulo DHT11
 * 
 * Aquí se encuentra la implementación de funciones, los defines, macros, 
 * datos internos y externos, declaraciones y definiciones de funciones
 * internas y definiciones de funciones externas
 */

/*==================[inclusiones]============================================*/
#include <xc.h>
#include "dht11.h"

/*==================[definiciones y macros]==================================*/
// Timer0: Fosc/4 = 5MHz, sin prescaler = 0.2us por tick
// Timeout de 255 ticks = 51us (suficiente para detectar errores)
#define DHT11_TIMEOUT        (255)    
#define DHT11_DATA_SIZE      (5)

#define TRUE  1
#define FALSE 0

#define _XTAL_FREQ           (20000000L)

#define dht11_GPIO_Low()     PIN_DHT11 = 0; TRIS_DHT11 = 0;
#define dht11_GPIO_High()    TRIS_DHT11 = 1;
#define dht11_GPIO_Read()    PIN_DHT11

// Timer0: prescaler 1:4 para mejor resolución
// Con prescaler 1:4: tick = 0.8us
#define dht11_TMR_Reset()    TMR0 = 0
#define dht11_TMR_Read()     TMR0
#define dht11_TMR_Config()   OPTION_REGbits.T0CS = 0; OPTION_REGbits.PSA = 0; OPTION_REGbits.PS = 0b001;

/*==================[definiciones de datos internos]=========================*/
static uint8_t dht11_byte[DHT11_DATA_SIZE];
static uint8_t dht11_aux;

/*==================[definiciones de funciones internas]=====================*/
/**
 * @brief       Lee un byte enviado por el módulo DHT11
 * @return      1 si la recepción fue correcta
 *              0 si hubo timeout
 * @note        Guarda el resultado en una variable global auxiliar (dht11_aux)
 */
static uint8_t dht11_read_byte() {
    uint8_t i;
    uint8_t timer_val;
    dht11_aux = 0;
    
    // Recibo 8 bits (MSB primero)
    for(i = 0; i < 8; i++) {
        // Espero flanco ascendente (el pulso en bajo siempre es ~50us)
        dht11_TMR_Reset();
        while(!dht11_GPIO_Read()) {
            if(dht11_TMR_Read() > DHT11_TIMEOUT) return FALSE;
        }
        
        // Mido la duración del pulso en alto
        // Si el pulso dura ~26-28us es un '0'
        // Si el pulso dura ~70us es un '1'
        dht11_TMR_Reset();
        while(dht11_GPIO_Read()) {
            if(dht11_TMR_Read() > DHT11_TIMEOUT) return FALSE;
        }
        
        timer_val = dht11_TMR_Read();
        
        // Desplazo el bit
        dht11_aux <<= 1;
        
        // Con prescaler 1:4: 0.8us por tick
        // Pulso '0' = ~28us = 35 ticks
        // Pulso '1' = ~70us = 87 ticks
        // Umbral: 50 ticks = 40us (punto medio)
        if(timer_val > 50) {
            dht11_aux |= 0x01;
        }
    }
    return TRUE;
}

/*==================[definiciones de funciones externas]=====================*/
/**
 * @brief       Configura e inicializa el pin de comunicación y el timer
 * @return      Nada
 */
void dht11_config(void) {
    dht11_GPIO_High();
    dht11_TMR_Config();
}

/**
 * @brief       Lee los datos del módulo DHT11
 * @param[in]   *phum: Dirección de la variable donde guardar la humedad
 * @param[in]   *ptemp: Dirección de la variable donde guardar la temperatura
 * @return      1 si la recepción fue correcta
 *              0 si hubo timeout o error de checksum
 */
uint8_t dht11_read(float *phum, float *ptemp) {
    uint8_t i;
    uint8_t checksum;
    
    // Señal de inicio: mínimo 18ms en bajo según datasheet
    dht11_GPIO_Low();
    __delay_ms(20);  // 20ms para asegurar
    
    // Liberar el bus y esperar respuesta del sensor
    dht11_GPIO_High();
    __delay_us(30);  // Esperar 20-40us
    
    // Esperar respuesta del DHT11: flanco descendente (~80us en alto)
    dht11_TMR_Reset();
    while(dht11_GPIO_Read()) {
        if(dht11_TMR_Read() > DHT11_TIMEOUT) {
            return FALSE;  // Timeout esperando respuesta
        }
    }
    
    // Esperar flanco ascendente: DHT11 mantiene bajo por ~80us
    dht11_TMR_Reset();
    while(!dht11_GPIO_Read()) {
        if(dht11_TMR_Read() > DHT11_TIMEOUT) {
            return FALSE;  // Timeout en señal de respuesta
        }
    }
    
    // Esperar flanco descendente: DHT11 mantiene alto por ~80us
    dht11_TMR_Reset();
    while(dht11_GPIO_Read()) {
        if(dht11_TMR_Read() > DHT11_TIMEOUT) {
            return FALSE;  // Timeout en señal de respuesta
        }
    }
    
    // Ahora el DHT11 enviará 40 bits de datos (5 bytes)
    for(i = 0; i < DHT11_DATA_SIZE; i++) {
        if(!dht11_read_byte()) {
            return FALSE;  // Error leyendo byte
        }
        dht11_byte[i] = dht11_aux;
    }
    
    // Verificar checksum
    checksum = dht11_byte[0] + dht11_byte[1] + dht11_byte[2] + dht11_byte[3];
    if(checksum != dht11_byte[4]) {
        return FALSE;  // Error de checksum
    }
    
    // Formatear los datos
    *phum  = ((float)dht11_byte[0]) + ((float)dht11_byte[1]) / 10.0;
    *ptemp = ((float)dht11_byte[2]) + ((float)dht11_byte[3]) / 10.0;
    
    return TRUE;
}

/*==================[fin del archivo]========================================*/