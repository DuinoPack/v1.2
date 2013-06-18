/*

Accelerometer ADXL345 demo program - V0.1
Federico Lanza for RobotGroup

Changes:
  V0.1: May 2012                First release
  V0.2: May 2012                Write and Read Methods are now general for every I2C device

*/

#include <I2cMaster.h>
#include <Softi2c.h> 
#include <ADXL345.h>

// Accelerometer
ADXL345 Accel;

void setup()
{
  delay(5000);  
  Serial.begin(9600);
  Serial.println("init");
}

void loop()
{
  int16_t x, y, z;
  
  Accel.getAccel(&x, &y, &z);
  Serial.print("x: "); Serial.print(x);
  Serial.print(" y: "); Serial.print(y);
  Serial.print(" z: "); Serial.println(z);
  delay(100);
}
