/*

Accel.h - Library for ADXL345 via Software I2C - V0.1
Federico Lanza for RobotGroup

Notes:
Connection for ADXL345 <--> DuinoBot
Gnd      -  GND
5v       -  VCC
5v       -  CS
Analog 4 -  SDA
Analog 5 -  SLC

Changes:
  V0.1: May 2012                First release
  V0.2: May 2012                Write and Read Methods are now general for every I2C device

*/

#ifndef ADXL345_H
#define ADXL345_H

#include "Softi2c.h"

// ADXL243 I2C Device Address
#define ADXL345_I2C_ADDR         0xA6

// ADXL345 Registers
#define ADXL345_DEVID            0x00
#define ADXL345_RESERVED1        0x01
#define ADXL345_THRESH_TAP       0x1d
#define ADXL345_OFSX             0x1e
#define ADXL345_OFSY             0x1f
#define ADXL345_OFSZ             0x20
#define ADXL345_DUR              0x21
#define ADXL345_LATENT           0x22
#define ADXL345_WINDOW           0x23
#define ADXL345_THRESH_ACT       0x24
#define ADXL345_THRESH_INACT     0x25
#define ADXL345_TIME_INACT       0x26
#define ADXL345_ACT_INACT_CTL    0x27
#define ADXL345_THRESH_FF        0x28
#define ADXL345_TIME_FF          0x29
#define ADXL345_TAP_AXES         0x2a
#define ADXL345_ACT_TAP_STATUS   0x2b
#define ADXL345_BW_RATE          0x2c
#define ADXL345_POWER_CTL        0x2d
#define ADXL345_INT_ENABLE       0x2e
#define ADXL345_INT_MAP          0x2f
#define ADXL345_INT_SOURCE       0x30
#define ADXL345_DATA_FORMAT      0x31
#define ADXL345_DATAX0           0x32
#define ADXL345_DATAX1           0x33
#define ADXL345_DATAY0           0x34
#define ADXL345_DATAY1           0x35
#define ADXL345_DATAZ0           0x36
#define ADXL345_DATAZ1           0x37
#define ADXL345_FIFO_CTL         0x38
#define ADXL345_FIFO_STATUS      0x39

class ADXL345
{
  private:
    // Buffer
    uint8_t Buff[6];
  public:
    ADXL345();
    void getAccel (int16_t *x, int16_t *y, int16_t *z);
    void setRangeSetting (uint8_t Val);
    uint8_t getRangeSetting ();
};

#endif
