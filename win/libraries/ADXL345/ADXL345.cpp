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

#include "ADXL345.h"

/*
Constructor
Initializes accelerometer, powers on, etc
*/
ADXL345::ADXL345()
{
  // Power On ADXL345
  Buff[0] = 0x00;
  WriteSoftI2C(ADXL345_I2C_ADDR, ADXL345_POWER_CTL, Buff, 1);
  Buff[0] = 0x10;
  WriteSoftI2C(ADXL345_I2C_ADDR, ADXL345_POWER_CTL, Buff, 1);
  Buff[0] = 0x08;
  WriteSoftI2C(ADXL345_I2C_ADDR, ADXL345_POWER_CTL, Buff, 1);
}

/*
Get acceleration data for x, y and z axis
*/
void ADXL345::getAccel (int16_t *x, int16_t *y, int16_t *z)
{
  // Read Acceleration data
  ReadSoftI2C(ADXL345_I2C_ADDR, ADXL345_DATAX0, Buff, 6);

  // 1 Axis = 2 bytes in 10 bit resolution (LSB First)
  *x = (((int)Buff[1]) << 8) | Buff[0];   
  *y = (((int)Buff[3]) << 8) | Buff[2];
  *z = (((int)Buff[5]) << 8) | Buff[4];
}

/*
Get Range setting from accelerometer
Posible values are 2, 4, 8 or 16
*/
uint8_t ADXL345::getRangeSetting ()
{
  uint8_t b;
  
  ReadSoftI2C(ADXL345_I2C_ADDR, ADXL345_DATA_FORMAT, &b, 1);
  return (b & 0x03);
}

/*
Sets Range setting into accelerometer
Posible values are 2, 4, 8 or 16
*/
void ADXL345::setRangeSetting (uint8_t Val)
{
  uint8_t s, b;
  
  switch (Val)
  {
    case 2:
      s = 0x00;
      break;
    case 4:
      s = 0x01;
      break;
    case 8:
      s = 0x02;
      break;
    case 16:
      s = 0x03;
      break;
    default:
      s = 0x00;
  }
  ReadSoftI2C(ADXL345_I2C_ADDR, ADXL345_DATA_FORMAT, &b, 1);
  s |= (b & 0xEC);
  WriteSoftI2C(ADXL345_I2C_ADDR, ADXL345_DATA_FORMAT, &s, 1);
}
