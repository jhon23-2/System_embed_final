/*============================================================================
 * Licencia:
 * Autor:       Nahuel Espinosa (Adaptado para PIC16F887 @ 20MHz)
 * Fecha:       20/10/2019
 *===========================================================================*/
/** @file
 * @brief    Contiene macros y definiciones para el módulo DS1307 RTC
 * 
 * Librería adaptada para usar con i2c.h/i2c.c existente
 */
#ifndef _DS1307_H_
#define _DS1307_H_

/*==================[inclusiones]============================================*/
#include <stdint.h>

/*==================[declaraciones de funciones externas]====================*/

/**
 * @brief Configura el DS1307 (debe llamarse después de I2C_Init_Master)
 */
void DS1307_Init(void);

/**
 * @brief Establece la fecha en el RTC
 * @param day   Día (1-31)
 * @param month Mes (1-12)
 * @param year  Año (00-99, representa 2000-2099)
 */
void DS1307_SetDate(uint8_t day, uint8_t month, uint8_t year);

/**
 * @brief Establece la hora en el RTC
 * @param hour Hora (0-23)
 * @param min  Minutos (0-59)
 * @param sec  Segundos (0-59)
 */
void DS1307_SetTime(uint8_t hour, uint8_t min, uint8_t sec);

/**
 * @brief Obtiene la fecha del RTC
 * @param day   Puntero donde guardar el día
 * @param month Puntero donde guardar el mes
 * @param year  Puntero donde guardar el año
 */
void DS1307_GetDate(uint8_t *day, uint8_t *month, uint8_t *year);

/**
 * @brief Obtiene la hora del RTC
 * @param hour Puntero donde guardar la hora
 * @param min  Puntero donde guardar los minutos
 * @param sec  Puntero donde guardar los segundos
 */
void DS1307_GetTime(uint8_t *hour, uint8_t *min, uint8_t *sec);

/*==================[fin del archivo]========================================*/
#endif /* _DS1307_H_ */