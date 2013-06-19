/*

LSM303 - Demo program for Accelerometer and Magnetometer LSM303
Federico Lanza for RobotGroup

Notes:
Connection for DEVICE <--> DuinoBot
Analog 4 -  Device SDA
Analog 5 -  Device SLC

Changes:
  V0.1: May 2012                First release

*/ 

#include <I2cMaster.h>
#include <Softi2c.h>
#include <LSM303.h>

// Arrays to store values
int16_t AccelValues[3];
int16_t MagValues[3];
float RealAccel[3];

LSM303 Accel;

void setup()
{
  delay(5000);
  Serial.begin(9600);
}

void loop()
{
  // Get accelerometer values
  Accel.getAccelerometer(AccelValues);
  // Calculate real acceleration values, in units of g
  Accel.getRealAccelerometer(RealAccel);

  // To get Raw accelerometer data uncomment the following lines
  /*
  Serial.print("x: ");
  Serial.println(AccelValues[X]);
  Serial.print(" y: ");
  Serial.print(AccelValues[Y]);
  Serial.print(" z: ");
  Serial.println(AccelValues[Z]);
  */

  Serial.print("x: ");
  Serial.print(RealAccel[X], 2);
  Serial.print(" y: ");
  Serial.print(RealAccel[Y], 2);
  Serial.print(" z: ");
  Serial.println(RealAccel[Z], 2);

  // Wait for Magnetometer data to be ready
  while (!Accel.IsMagnetometerReady());
  Accel.getMagnetometer(MagValues);

  Serial.print("mx: ");
  Serial.print(MagValues[X], DEC);
  Serial.print(" my: ");
  Serial.print(MagValues[Y], DEC);
  Serial.print(" mz: ");
  Serial.println(MagValues[Z], DEC);

  Serial.print("H: ");
  // The following value is valid only with sensor leveled
  Serial.print(Accel.getHeading(MagValues), 3);
  Serial.print("  TH: ");
  // With Tilt Compensation, Heading is obtained no matter sensor is leveled or not
  Serial.println(Accel.getTCHeading(MagValues, RealAccel), 3);
  
  delay(200);
}
