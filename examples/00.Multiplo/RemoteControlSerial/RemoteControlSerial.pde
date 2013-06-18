#include <IRremote.h>

//Use this example with the Multiplo IR 38 KHz sensor and a standard RC5 remote control:

IRrecv irReceiver(A0); //S0 input

void setup()
{
   Serial.begin(9600);
}

void loop()
{
   Serial.println(irReceiver.getIRRemoteCode());
   delay(100);
}