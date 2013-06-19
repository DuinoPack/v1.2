#include <Ping.h>


PingSensor ping(A1); //S1 input.


void setup()
{
  Serial.begin(9600);
}


void loop()
{
  Serial.println(ping.measureCM());
  delay(200);
}

