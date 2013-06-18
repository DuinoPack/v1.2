/*

IDG500 - Library for Gyroscope IDG500
Federico Lanza for RobotGroup

Notes:
Connection IDG500 <------> DuinoBot
-----------------------------------
VCC --- 5V
GND --- GND
XOUT --- ANALOG 4
YOUT --- ANALOG 5

Changes:
  V0.1: Apr 2012                First release

*/

#include "wiring.h"
#include "IDG500.h"

// Default Constructor
// Supossed that sensor is static at the moment it is called
IDG500::IDG500 ()
{
  int16_t XValue, YValue;

  // Read values for Zero Reference (supossing sensor is at rest)
  ZeroReferenceX = analogRead(IDG500_XAXIS_PIN);
  ZeroReferenceY = analogRead(IDG500_YAXIS_PIN);  
}

// Constructor
// It sets Zero reference out given in ZroX and ZroY
IDG500::IDG500 (int16_t ZroX, int16_t ZroY)
{
  // Set references
  ZeroReferenceX = ZroX;
  ZeroReferenceY = ZroY;
}

// Set reference values 
void IDG500::setReferences (int16_t ZroX, int16_t ZroY)
{
  // Set references
  ZeroReferenceX = ZroX;
  ZeroReferenceY = ZroY;
}

// Set references with current values, supossing sensor is at rest
void IDG500::setCurrentAsReferences ()
{
  // Read values for Zero Reference (supossing sensor is at rest)
  ZeroReferenceX = analogRead(IDG500_XAXIS_PIN);
  ZeroReferenceY = analogRead(IDG500_YAXIS_PIN);  
}

// Returns X-Axis Angular Rate
float IDG500::getXAngularRate ()
{
  int16_t XValue;
  
  XValue = analogRead(IDG500_XAXIS_PIN);
  return (((XValue - ZeroReferenceX) * IDG500_RESOLUTION) / IDG500_SENSITIVITY);
}

// Returns Y-Axis Angular Rate
float IDG500::getYAngularRate ()
{
  int16_t YValue;

  YValue = analogRead(IDG500_YAXIS_PIN);
  return (((YValue - ZeroReferenceY) * IDG500_RESOLUTION) / IDG500_SENSITIVITY);
}
