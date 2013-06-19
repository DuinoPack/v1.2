/*

Softi2c - Library for accessing I2C via Software - V0.1
Federico Lanza for RobotGroup

Notes:
Connection for DEVICE <--> DuinoBot
Analog 4 -  Device SDA
Analog 5 -  Device SLC

Changes:
  V0.1: May 2012                First release

*/

#include "Softi2c.h"

// Software I2C Control
SoftI2cMaster i2c(SDA_PIN, SCL_PIN);

/*
Write to Device in DeviceAddr address, to register specified in Address, with content Buf, with Len bytes
*/
bool WriteSoftI2C (uint8_t DeviceAddr, uint8_t Address, uint8_t *Buf, uint8_t Len)
{
  // Start condition, send device address and write direction bit
  if (!i2c.start(DeviceAddr | I2C_WRITE) | !i2c.restart(DeviceAddr | I2C_WRITE)) return false;
  // Register address
  if (!i2c.write(Address)) return false;
  // Send data
  for (uint8_t i = 0; i < Len; i++)
  {
    if (!i2c.write(Buf[i])) return false;
  }
  // Stop condition
  i2c.stop();
  return true;
}

/*
Read from Device in DeviceAddr address, from register specified in Address, content is located in Buf, with Len bytes
*/
bool ReadSoftI2C (uint8_t DeviceAddr, uint8_t Address, uint8_t *Buf, uint8_t Len)
{
  // Start condition, send device address and write direction bit
  if (!i2c.start(DeviceAddr | I2C_WRITE)) return false;
  // Register address
  if (!i2c.write(Address)) return false;
  // Repeated start condition, send device address and read direction bit
  if (!i2c.restart(DeviceAddr | I2C_READ)) return false;
  // Read data
  for (uint8_t i = 0; i < Len; i++)
  {
    // send Ack until last byte then send Ack
    Buf[i] = i2c.read(i == (Len - 1));
  }
  // Stop condition
  i2c.stop();
  return true;
}

