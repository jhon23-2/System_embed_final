/*============================================================================
 * Licencia:
 * Autor:       Nahuel Espinosa
 * Fecha:       20/10/2019
 *===========================================================================*/
/** @file
 * @brief	Contiene macros y definiciones para el m?dulo DHT11
 * 
 * Aqui se definen macros, tipos de datos y declaraci?n de funciones
 */
#ifndef _DHT11_H_
#define _DHT11_H_

/*==================[inclusiones]============================================*/
#include <stdint.h>

/*==================[macros]=================================================*/
#define PIN_DHT11   PORTAbits.RA0
#define TRIS_DHT11  TRISAbits.TRISA0

/*==================[declaraciones de funciones externas]====================*/
void dht11_config (void);
uint8_t dht11_read (float *phum, float *ptemp);

/*==================[fin del archivo]========================================*/
#endif /* _DHT11_H_ */