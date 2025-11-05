/*============================================================================
 * DS1307 RTC Library - Adaptado para usar i2c.h
 * Compatible con PIC16F887 y XC8
 *===========================================================================*/
#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

// Funciones públicas del DS1307
void ds1307_init(void);
void ds1307_set_date(uint8_t day, uint8_t month, uint8_t year);
void ds1307_set_time(uint8_t hour, uint8_t min, uint8_t sec);
void ds1307_get_date(uint8_t *day, uint8_t *month, uint8_t *year);
void ds1307_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec);

#endif /* DS1307_H */