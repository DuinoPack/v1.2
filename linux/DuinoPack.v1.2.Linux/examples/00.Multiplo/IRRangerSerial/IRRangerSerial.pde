#include <IRRanger.h>


//Select your type of ranger sensor:
//IRRanger irRanger(0, IRRanger::range10to80cm); //S0 input, 80 cm ranger sensor.
IRRanger irRanger(1, IRRanger::range20to150cm); //S1 input, 150 cm ranger sensor.


void setup()
{
  Serial.begin(115200);
}


void loop()
{
  Serial.println(irRanger.measureCM());
  delay(200);
}