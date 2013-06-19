/*

IDG500 - Demo program for Gyroscope IDG500
Federico Lanza for RobotGroup

Changes:
  V0.1: Apr 2012                First release

*/

#include "IDG500.h"

IDG500 Gyro;

void setup()
{
  Serial.begin(9600);
  delay(100);
  Gyro.setCurrentAsReferences();
}

void loop()
{
  float X, Y;
  
  X = Gyro.getXAngularRate();
  Y = Gyro.getYAngularRate();
  Serial.print("x: "); Serial.print(X, 1);
  Serial.print("  y: "); Serial.println(Y, 1);
  delay(100);
}





