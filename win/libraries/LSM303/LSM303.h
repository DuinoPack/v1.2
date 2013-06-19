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

#ifndef LSM303_H
#define LSM303_H

#include "Softi2c.h"

// Accel default scale, should be 2, 4, or 8
#define LSM303_ACC_DEFAULTSCALE    2

// LSM303 I2C Address definitions
#define LSM303_MAG_DEVICEADDR  0x3C
#define LSM303_ACC_DEVICEADDR  0x30  // Error in datasheet?? it proposes 0x32

#define X 0
#define Y 1
#define Z 2

// LSM303 Register definitions
// Acceleormeter Registers
#define CTRL_REG1_A          0x20
#define CTRL_REG2_A          0x21
#define CTRL_REG3_A          0x22
#define CTRL_REG4_A          0x23
#define CTRL_REG5_A          0x24
#define HP_FILTER_RESET_A    0x25
#define REFERENCE_A          0x26
#define STATUS_REG_A         0x27
#define OUT_X_L_A            0x28
#define OUT_X_H_A            0x29
#define OUT_Y_L_A            0x2A
#define OUT_Y_H_A            0x2B
#define OUT_Z_L_A            0x2C
#define OUT_Z_H_A            0x2D
#define INT1_CFG_A           0x30
#define INT1_SOURCE_A        0x31
#define INT1_THS_A           0x32
#define INT1_DURATION_A      0x33
// Magnetometer Registers
#define CRA_REG_M            0x00
#define CRB_REG_M            0x01
#define MR_REG_M             0x02
#define OUT_X_H_M            0x03
#define OUT_X_L_M            0x04
#define OUT_Y_H_M            0x05
#define OUT_Y_L_M            0x06
#define OUT_Z_H_M            0x07
#define OUT_Z_L_M            0x08
#define SR_REG_M             0x09
#define IRA_REG_M            0x0A
#define IRB_REG_M            0x0B
#define IRC_REG_M            0x0C

class LSM303
{
  private:
    uint8_t Scale;
    void Init(int16_t fs);
  public:
    LSM303 ();
    LSM303 (int16_t fs);
    bool IsMagnetometerReady ();
    void getMagnetometer (int16_t *Values);
    void getAccelerometer (int16_t *Values);
    void getRealAccelerometer (float *Values);
    float getHeading(int16_t *magValue);
    float getTCHeading(int16_t *magValue, float *accelValue);
};

#endif
