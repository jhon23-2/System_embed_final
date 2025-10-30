/* 
 * File: i2c.h
 * I2C Library for PIC16F887
 * Compatible con DHT11, DS1307 y LCD I2C
 */
#ifndef I2C_H
#define I2C_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 20000000

#define TRIS_SCL TRISCbits.TRISC3 
#define TRIS_SDA TRISCbits.TRISC4 

#define I2C_100KHZ 0x80
#define I2C_400KHZ 0x00

#define I2C_MASTER_MODE

#ifdef I2C_MASTER_MODE

void I2C_Init_Master(unsigned char sp_i2c);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Restart(void);
void I2C_Ack(void);
void I2C_Nack(void);
unsigned char I2C_Read(void);
uint8_t I2C_Write(char data);  // Retorna 0 si ACK, 1 si NACK

#endif

#endif /* I2C_H */