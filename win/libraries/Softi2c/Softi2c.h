/*

Softi2c - Library for accessing I2C via Software - V0.1
Federico Lanza for RobotGroup

Notes:
Connection for DEVICE <--> DuinoBot
Analog 4 -  Device SDA
Analog 5 -  Device SLC
Can be changed with SCL_PIN and SDA_PIN defines

Changes:
  V0.1: May 2012                First release

*/

#ifndef SOFTI2C_H
#define SOFTI2C_H

#include "I2cMaster.h"

#define SCL_PIN A5
#define SDA_PIN A4

bool WriteSoftI2C (uint8_t DeviceAddr, uint8_t Address, uint8_t *Buf, uint8_t Len);
bool ReadSoftI2C (uint8_t DeviceAddr, uint8_t Address, uint8_t *Buf, uint8_t Len);

#endif
