/* 
 * File: i2c.c
 * I2C Library Implementation for PIC16F887
 * Sin warnings de compilación
 */
#include "i2c.h"

#ifdef I2C_MASTER_MODE

void I2C_Init_Master(unsigned char sp_i2c)
{
    TRIS_SCL = 1;
    TRIS_SDA = 1;
    
    SSPSTAT = sp_i2c;
    SSPCON = 0x28; 
    SSPCON2 = 0x00;
    
    if(sp_i2c == I2C_100KHZ){
        SSPADD = 49;
    }
    else if(sp_i2c == I2C_400KHZ){
        SSPADD = 12;
    }
}

void I2C_Start(void)
{
    SSPCON2bits.SEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Stop(void)
{
    SSPCON2bits.PEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Restart(void)
{
    SSPCON2bits.RSEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Ack(void)
{
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Nack(void)
{
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

uint8_t I2C_Write(char data)
{
    SSPBUF = data;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    return (uint8_t)SSPCON2bits.ACKSTAT;
}

unsigned char I2C_Read(void)
{
    SSPCON2bits.RCEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
    return SSPBUF;
}

#endif