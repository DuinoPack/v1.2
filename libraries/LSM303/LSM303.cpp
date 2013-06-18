/*

LSM303 - Library for Accelerometer and Magnetometer LSM303
Federico Lanza for RobotGroup

Notes:
Connection for DEVICE <--> DuinoBot
Analog 4 -  Device SDA
Analog 5 -  Device SLC

Changes:
  V0.1: May 2012                First release

*/

#include "LSM303.h"

/*
Private method for LSM303 initialization
called from constructors
*/
void LSM303::Init (int16_t fs)
{
  uint8_t B;

  // 0x27 = normal power mode, all accel axes on
  B = 0x27;
  WriteSoftI2C(LSM303_ACC_DEVICEADDR, CTRL_REG1_A, &B, 1);

  // Scale
  if ((fs == 8) || (fs == 4))
    // Set Full scale
    B = (0x00 | (fs-fs/2-1)<<4);
  else
    B = 0x00;
  WriteSoftI2C(LSM303_ACC_DEVICEADDR, CTRL_REG4_A, &B, 1);

  // 0x14 = mag 30Hz output rate
  B = 0x14;  
  WriteSoftI2C(LSM303_MAG_DEVICEADDR, CRA_REG_M, &B, 1);
  
  // 0x00 = continouous conversion mode
  B = 0x00;
  WriteSoftI2C(LSM303_MAG_DEVICEADDR, MR_REG_M, &B, 1);
}

/*
Constructor
Initializes LSM303 Scale as defined in LSM303_ACC_DEFAULTSCALE
*/
LSM303::LSM303 ()
{
  // Set Scale property
  Scale = LSM303_ACC_DEFAULTSCALE;
  // Call init method
  Init(Scale);
}

/*
Constructor
Initializes LSM303 with fs as Scale.
fs must be 2, 4 or 8
*/
LSM303::LSM303 (int16_t fs)
{
  // Set Scale property
  Scale = fs;
  // Call init method
  Init(Scale);
}

/*
Returns true if Magnetometer readings are ready
oitherwise it returns false
*/
bool LSM303::IsMagnetometerReady ()
{
  uint8_t SrReg;
  
  // Read SR Register
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, SR_REG_M, &SrReg, 1);
  // Bit 0 == DRDY
  return ((SrReg & 0x01) ? true : false);
}

/*
Reads and returns Magnetometer raw values
Values[0] = X Axis
Values[1] = Y Axis
Values[2] = Z Axis
*/
void LSM303::getMagnetometer (int16_t *Values)
{
  uint8_t LoB, HiB;
  
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_X_H_M, &HiB, 1);
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_X_L_M, &LoB, 1);
  Values[X] = (((int16_t)HiB) << 8) | ((int16_t)LoB);
  
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_Y_H_M, &HiB, 1);
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_Y_L_M, &LoB, 1);
  Values[Y] = (((int16_t)HiB) << 8) | ((int16_t)LoB);

  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_Z_H_M, &HiB, 1);
  ReadSoftI2C(LSM303_MAG_DEVICEADDR, OUT_Z_L_M, &LoB, 1);
  Values[Z] = (((int16_t)HiB) << 8) | ((int16_t)LoB);
}

/*
Reads and returns Accelerometer raw values
Values[0] = X Axis
Values[1] = Y Axis
Values[2] = Z Axis
*/
void LSM303::getAccelerometer (int16_t *Values)
{
  uint8_t LoB, HiB;
  
  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_X_L_A, &LoB, 1);
  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_X_H_A, &HiB, 1);
  Values[X] = (((int16_t)HiB) << 8) | ((int16_t)LoB);
  
  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_Y_L_A, &LoB, 1);
  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_Y_H_A, &HiB, 1);
  Values[Y] = (((int16_t)HiB) << 8) | ((int16_t)LoB);

  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_Z_L_A, &LoB, 1);
  ReadSoftI2C(LSM303_ACC_DEVICEADDR, OUT_Z_H_A, &HiB, 1);
  Values[Z] = (((int16_t)HiB) << 8) | ((int16_t)LoB);
}

/*
Reads and returns Accelerometer Real values, based on scale
RealValues are in the range -1..1
RealValues[0] = X Axis
RealValues[1] = Y Axis
RealValues[2] = Z Axis
*/
void LSM303::getRealAccelerometer (float *RealValues)
{
  int16_t Buff[3];
  
  getAccelerometer(Buff);
  for (int i = 0; i < 3; i++)
    RealValues[i] = Buff[i] / pow(2, 15) * Scale;
}

/*
Calculates North Heading assuming sensor is leveled (that is rarely true)

magValue must be the Values returned by the magnetometer (see getMagnetometer)
magValue[0] = X Axis
magValue[1] = Y Axis
magValue[2] = Z Axis
*/
float LSM303::getHeading(int16_t *magValue)
{
  // Assume pitch and roll are 0
  float heading = 180 * atan2(magValue[Y], magValue[X]) / PI;
  if (heading < 0) heading += 360;
  
  return heading;
}

/*
Calculates North Heading with Tilt Compensation

magValue must be the Values returned by the magnetometer (see getMagnetometer)
magValue[0] = X Axis
magValue[1] = Y Axis
magValue[2] = Z Axis

accelValue must be the real acceleration values (see getRealAccelerometer)
accelValue[0] = X Axis
accelValue[1] = Y Axis
accelValue[2] = Z Axis
*/
float LSM303::getTCHeading(int16_t *magValue, float *accelValue)
{
  // Compensate with pitch and roll
  float pitch = asin(-accelValue[X]);
  float roll = asin(accelValue[Y] / cos(pitch));
  
  float xh = magValue[X] * cos(pitch) + magValue[Z] * sin(pitch);
  float yh = magValue[X] * sin(roll) * sin(pitch) + magValue[Y] * cos(roll) - magValue[Z] * sin(roll) * cos(pitch);
  float zh = -magValue[X] * cos(roll) * sin(pitch) + magValue[Y] * sin(roll) + magValue[Z] * cos(roll) * cos(pitch);

  float heading = 180 * atan2(yh, xh) / PI;
  if (yh >= 0)
    return heading;
  else
    return (360 + heading);
}
